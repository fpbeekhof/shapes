/***************************************************************************
 *   Copyright (C) 2009 by F. P. Beekhof                                   *
 *   fpbeekhof@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with program; if not, write to the                              *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SHAPES_EXPORT_OCTREE_H
#define SHAPES_EXPORT_OCTREE_H 1

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
//#include <boost/iostreams/filter/zlib.hpp>

#include <cvmlcpp/base/Matrix>
#include <cvmlcpp/math/Math>
#include <cvmlcpp/volume/DTree>
#include <cvmlcpp/volume/Voxelizer>

#include <shapes/Shape.h>

namespace shapes {

namespace detail
{
template <typename T>
void shapeToZBuffer(const Shape<T> &shape, const unsigned axis,
		     const std::size_t dimX, const std::size_t dimY, const std::size_t dimZ,
		     const T &deltaX, const T &deltaY, const T &deltaZ,
		     const T sampleSize,
		     cvmlcpp::Matrix<std::vector<double>, 2u> &zBuffer)
{
	const unsigned _dm[][4] =
	{
		{Z, Y, X, 0u},
		{X, Z, Y, 0u},
		{X, Y, Z, 0u}
	};

	const unsigned XX = _dm[axis][X];
	const unsigned YY = _dm[axis][Y];
	const unsigned ZZ = _dm[axis][Z];

	const std::size_t dim [] = {dimX, dimY, dimZ};
	const T delta [] = {deltaX, deltaY, deltaZ};

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int x = 0; x < int(dim[XX]); ++x)
	for (std::size_t y = 0; y < dim[YY]; ++y)
	{
		T    prevField = 0.;
		bool prevValue = false;
		for (std::size_t z = 0; z < dim[ZZ]; ++z)
		{
			assert(prevValue == (prevField >= T(1)));

			typename Shape<T>::FPPoint p;
			p[XX] = T(x)*sampleSize + delta[XX];
			p[YY] = T(y)*sampleSize + delta[YY];
			p[ZZ] = T(z)*sampleSize + delta[ZZ];

			const T    currentField = shape.value(p);
			const bool currentValue = currentField >= T(1);

			if (prevValue != currentValue)
			{
				// Interpolation to find crossing point
				const T d = (currentField - T(1)) /
					// ----------------------------
					    (currentField - prevField);
				assert(d >= 0);

				const T z_in_space = p[ZZ]-sampleSize*d;
				assert( z_in_space <= p[ZZ]);
				assert( z_in_space >= p[ZZ]-sampleSize);

				zBuffer[x][y].push_back(z_in_space - delta[ZZ]);
				assert(zBuffer[x][y].back() >= 0.);

				prevValue = currentValue;
			}
			prevField = currentField;
		}
	}
}

} // end namespace detail

template <typename T, typename V>
bool convertToOctree(const Shape<T> &shape, const T sampleSize,
			cvmlcpp::DTree<V, 3> &voxtree)
{
	if (shape.empty())
	{
		voxtree.collapse(0);
		return true;
	}

	std::size_t dimX, dimY, dimZ;
	T deltaX, deltaY, deltaZ;
	calcShapeConsts(shape, sampleSize, dimX, dimY, dimZ,
			deltaX, deltaY, deltaZ);

	const std::size_t maxDim    = std::max(dimX, std::max(dimY, dimZ));
	const std::size_t dimension = cvmlcpp::isPower2(maxDim) ?
				maxDim : (2u << cvmlcpp::log2(maxDim));
	const std::size_t matrixDims [] = { dimension, dimension };
	cvmlcpp::Matrix<std::vector<double>, 2u> zBuffers[3];
	for (unsigned axis = 0; axis < 3; ++axis)
	{
		zBuffers[axis].resize(matrixDims);

		for (std::size_t x = 0u; x < dimension; ++x)
		for (std::size_t y = 0u; y < dimension; ++y)
			assert(zBuffers[axis][x][y].empty());

		// Build Z-Buffer using projection along given axis
		detail::shapeToZBuffer( shape, axis, dimX, dimY, dimZ,
					deltaX, deltaY, deltaZ,
					sampleSize, zBuffers[axis]);
	}

	// Abuse cvmlcpp internals ... Not chique
	const cvmlcpp::fVector3D subVoxOffset = 0.5;
	cvmlcpp::detail::zBuffersToDTree_(voxtree, sampleSize, subVoxOffset, zBuffers, V(1), V(0));

	return true;
}

namespace io {

template <typename T>
bool exportOctree(const std::string fileName, const Shape<T> &shape,
		  const T sampleSize = T(1))
{
	cvmlcpp::DTree<short int, 3> voxtree;
	if (!convertToOctree(shape, sampleSize, voxtree))
		return false;

	std::ofstream f(fileName.c_str(), std::ios::trunc);
	boost::iostreams::filtering_ostream out;
	out.push(boost::iostreams::gzip_compressor());
//	out.push(boost::iostreams::zlib_compressor());
	out.push(f);
	out << voxtree << std::endl;

	// 'f' is closed automatically upon destruction of 'out'
	return f.good();
}

} // end namespace io

} // end namespace shapes

#endif
