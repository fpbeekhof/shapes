/***************************************************************************
 *   Copyright (C) 2006 by F. P. Beekhof                                   *
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

#ifndef SHAPES_EXPORT_STL_H
#define SHAPES_EXPORT_STL_H 1

#include <string>
#include <cvmlcpp/base/Matrix>
#include <cvmlcpp/volume/Geometry>
#include <cvmlcpp/volume/SurfaceExtractor>
#include <cvmlcpp/volume/VolumeIO>

#include <shapes/Shape.h>

namespace shapes {

namespace detail {

template <typename T>
class ShapeSurfaceAdaptor
{
	public:
		typedef T value_type;

		ShapeSurfaceAdaptor(const Shape<T> &shape, const T sampleSize,
				const std::size_t dimX, const std::size_t dimY, const std::size_t dimZ,
				const T deltaX, const T deltaY, const T deltaZ) : shape_(shape),
				sampleSize_(sampleSize), dimX_(dimX), dimY_(dimY), dimZ_(dimZ),
				deltaX_(deltaX), deltaY_(deltaY), deltaZ_(deltaZ)
		{
			const std::size_t noKey = dimX_*dimY_*dimZ_;
			cache_.resize(dimZ);
			for (std::size_t z = 0; z < dimZ; ++z)
			for (int i = 0; i < 4; ++i)
				cache_[z][i].first = noKey;
		}

		T operator()(const std::size_t x, const std::size_t y, const std::size_t z) const
		{
			// Search in cache to see if we've recently computed this
			const std::size_t key = x*dimY_*dimZ_ + y*dimZ_ + z;

			const int index = ((x&0x1)<<1) | (y&0x1);

			if (cache_[z][index].first == key)
				return cache_[z][index].second;

			const typename Shape<T>::FPPoint
				 p( T(x)*sampleSize_ + deltaX_,
				    T(y)*sampleSize_ + deltaY_,
				    T(z)*sampleSize_ + deltaZ_ );
			const T value = shape_.value(p);

			// This is not needed as each thread should have it's own Adapter.
			// #ifdef _OPENMP
			// #pragma omp critical(SHAPES_EVAL)
			// #endif
			cache_[z][index].first  = key;
			cache_[z][index].second = value;
			return value;
		}

		template <typename It>
		void dims(It dims__) const
		{
			dims__[X] = dimX_;
			dims__[Y] = dimY_;
			dims__[Z] = dimZ_;
		}

	private:
		const Shape<T> &shape_;
		const T sampleSize_;
		const std::size_t dimX_, dimY_, dimZ_;
		const T deltaX_, deltaY_, deltaZ_;
		mutable std::vector<std::tr1::array<std::pair<std::size_t, T>, 4> > cache_;
};

} // end namespace detail

template <typename T>
bool convertToGeometry( const Shape<T> &shape, const T sampleSize,
			cvmlcpp::Geometry<T> &geometry)
{
	std::size_t dimX, dimY, dimZ;
	T deltaX, deltaY, deltaZ;
	calcShapeConsts(shape, sampleSize, dimX, dimY, dimZ, deltaX, deltaY, deltaZ);
	const detail::ShapeSurfaceAdaptor<T> sAdaptor =
			detail::ShapeSurfaceAdaptor<T>( shape, sampleSize,
							dimX, dimY, dimZ,
							deltaX, deltaY, deltaZ);
	cvmlcpp::extractSurfaceFromAdapter(sAdaptor, geometry, T(1));

//	geometry.scale(sampleSize);
	geometry.translate(deltaX, deltaY, deltaZ);

	return true;
}

namespace io
{

template <typename T>
bool exportSTL(const std::string fileName, const Shape<T> &shape,
		const T sampleSize = T(1))
{
	cvmlcpp::Geometry<T> geometry;
	if (!convertToGeometry(shape, sampleSize, geometry))
		return false;
	return cvmlcpp::writeSTL(geometry, fileName, true);
}

}// end namespace io

} // end namespace shape

#endif
