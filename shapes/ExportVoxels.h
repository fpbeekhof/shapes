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

#ifndef SHAPES_EXPORT_VOX_H
#define SHAPES_EXPORT_VOX_H 1

#include <omptl/omptl_algorithm>

#include <shapes/Shape.h>

namespace shapes {

namespace detail
{
	template <typename T, typename V>
	struct VoxValue
	{
		V operator()(const T x) const
		{ return (x >= T(1)) ? 1 : 0; }
	};
}

template <typename T, typename V>
bool convertToVoxels(const cvmlcpp::Matrix<T, 3> &field,
		     cvmlcpp::Matrix<V, 3> &voxels)
{
	voxels.resize(field.extents());
	omptl::transform( field.begin(), field.end(), voxels.begin(),
			  detail::VoxValue<T, V>() );
	return true;
}

template <typename T, typename V>
bool convertToVoxels(const Shape<T> &shape, const T sampleSize,
		     cvmlcpp::Matrix<V, 3> &voxels)
{
	cvmlcpp::Matrix<T, 3> field;
	return  convertToField(shape, sampleSize, field) &&
		convertToVoxels(field, voxels);
}

namespace io {

template <typename T>
bool exportVoxels(const std::string fileName, const cvmlcpp::Matrix<T, 3> &field)
{
	cvmlcpp::Matrix<char, 3> voxels;
	return  convertToVoxels(field, voxels) &&
		cvmlcpp::writeVoxels(voxels, fileName);
}

template <typename T>
bool exportVoxels(const std::string fileName, const Shape<T> &shape, const T sampleSize = T(1))
{
	cvmlcpp::Matrix<T, 3> field;
	return  convertToField(shape, sampleSize, field) &&
	 	exportVoxels(fileName, field);
}

}  // end namespace io

} // end namespace shapes

#endif
