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

#ifndef SHAPES_SHAPE_H
#define SHAPES_SHAPE_H 1

#include <vector>
#include <tr1/memory>

#include <shapes/tinyxml.h>
#include <shapes/Structure.h>

namespace shapes
{

template <typename T>
class Shape
{
	public:
		typedef typename Structure<T>::FPPoint  FPPoint;
		typedef typename Structure<T>::FPVector FPVector;

		Shape() : boxCached(false), boxSet(false) { }

		bool fromXml(TiXmlDocument &doc);

		TiXmlDocument * const toXml() const;

		// Do _not_ delete the structure yourself!!
		void add(Structure<T> * const structure);
		void clear();

		bool empty() const { return structure_ == NULL; }

		void getBoundingBox(FPPoint &_minCorner,
				    FPPoint &_maxCorner) const;

		void setBoundingBox(FPPoint &_minCorner,
				    FPPoint &_maxCorner)
		{
			minCorner = _minCorner;
			maxCorner = _maxCorner;
			boxSet    = true;
			boxCached = false;
		}

		bool boundingBoxIsSet() const { return boxSet; }

		void unSetBoundingBox() { boxSet = false; }

		void print() const;

		T value(const FPPoint &p) const
		{ return this->empty() ? 0.0 : structure_-> value(p); }

	private:
		mutable FPPoint minCorner, maxCorner;
		mutable bool boxCached;
		bool boxSet;
		std::tr1::shared_ptr<Structure<T> > structure_;
};

template <typename T>
void calcShapeConsts(const Shape<T> &shape, const T sampleSize,
		std::size_t &dimX, std::size_t &dimY, std::size_t &dimZ,
		T &deltaX, T &deltaY, T &deltaZ)
{
	typename Shape<T>::FPPoint minCorner, maxCorner;
	shape.getBoundingBox(minCorner, maxCorner);

// std::cout << "MinCorner "; minCorner.print(); std::cout << std::endl;
// std::cout << "MaxCorner "; maxCorner.print(); std::cout << std::endl;

	const std::size_t dx = std::floor((maxCorner.x() - minCorner.x()) / sampleSize) + 1;
	const std::size_t dy = std::floor((maxCorner.y() - minCorner.y()) / sampleSize) + 1;
	const std::size_t dz = std::floor((maxCorner.z() - minCorner.z()) / sampleSize) + 1;
	assert(dx >= 0);
	assert(dy >= 0);
	assert(dz >= 0);

// std::cout << "Sizes: ["<<dx<<", "<<dy<<", "<<dz<<"]" << std::endl;

	const int pad = 1;
// std::cout << "Padding: " << pad << std::endl;

	dimX = dx + 2 * pad;
	dimY = dy + 2 * pad;
	dimZ = dz + 2 * pad;

// std::cout << "Dimensions: ["<<dimX<<", "<<dimY<<", "<<dimZ<<"]" << std::endl;

	deltaX = minCorner.x() - T(pad) * sampleSize;
	deltaY = minCorner.y() - T(pad) * sampleSize;
	deltaZ = minCorner.z() - T(pad) * sampleSize;

// std::cout << "Offset: ["<<deltaX<<", "<<deltaY<<", "<<deltaZ<<"]"<<std::endl;
}

} // end namespace

#include <shapes/Shape.hh>

#endif
