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

#include <cmath>

namespace shapes
{

template <typename T>
bool convertToField(const Shape<T> &shape, const T sampleSize,
		   cvmlcpp::Matrix<T, 3> &field)
{
	if (shape.empty())
	{
		field.clear();
		return true;
	}

	std::size_t dimX, dimY, dimZ;
	T deltaX, deltaY, deltaZ;
	calcShapeConsts(shape, sampleSize, dimX, dimY, dimZ,
			deltaX, deltaY, deltaZ);

	// Init field
	const std::size_t dims [] = {dimX, dimY, dimZ};
	field.resize(dims);
	//~ field = 0.0;

	// Compute contributions of structures to the distance field
#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int x = 0; x < int(dimX); ++x)
	for (std::size_t y = 0; y < dimY; ++y)
	for (std::size_t z = 0; z < dimZ; ++z)
	{
		const typename Shape<T>::FPPoint
			 p( T(x)*sampleSize + deltaX,
			    T(y)*sampleSize + deltaY,
			    T(z)*sampleSize + deltaZ );

		field[x][y][z] = shape.value(p);
	}

	// Verification
#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int y = 0; y < int(dimY); ++y)
	for (std::size_t z = 0; z < dimZ; ++z)
		assert(field[0][y][z] < 1.0);
#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int y = 0; y < int(dimY); ++y)
	for (std::size_t z = 0; z < dimZ; ++z)
		assert(field[dimX-1][y][z] < 1.0);

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int x = 0; x < int(dimX); ++x)
	for (std::size_t z = 0; z < dimZ; ++z)
		assert(field[x][0][z] < 1.0);

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int x = 0; x < int(dimX); ++x)
	for (std::size_t z = 0; z < dimZ; ++z)
		assert(field[x][dimY-1][z] < 1.0);

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int x = 0; x < int(dimX); ++x)
	for (std::size_t y = 0; y < dimY; ++y)
		assert(field[x][y][0] < 1.0);

#ifdef _OPENMP
	#pragma omp parallel for
#endif
	for (int x = 0; x < int(dimX); ++x)
	for (std::size_t y = 0; y < dimY; ++y)
		assert(field[x][y][dimZ-1] < 1.0);

	return true;
}

} // end namespace

