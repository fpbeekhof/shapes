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

#ifndef SHAPES_SPHERE_H
#define SHAPES_SPHERE_H 1

#include <shapes/SphericStructure.h>
#include <shapes/Point.h>

namespace shapes
{

template <typename T>
class Sphere : public SphericStructure<T>, public Point<T>
{
	public:
		typedef typename EuclidTypes<T>::FPPoint  FPPoint;
		typedef typename EuclidTypes<T>::FPVector FPVector;

		Sphere(const std::string name__ = "") : SphericStructure<T>(name__)
		{
			// Three unit vectors
			for (unsigned i = 0; i < 3; ++i)
			{
				orientation[i]    = 0.0;
				orientation[i][i] = 1.0;
			}
		}

		Sphere(const FPPoint &_center,
			const FPVector &_weight,
//			const FPVector _orientation[3],
			const T _R,
			const FPVector _rotVector = 1.0f, const T _angle = 0.0, // Radians!!
			const T _exponent = 2.0, const std::string name__ = "") :
				Point<T>(_center, _weight, _R, _rotVector, _angle, _exponent),
				SphericStructure<T>(name__)
		{ Point<T>::recomputeOrientation(this->rotVector, this->angle, orientation); }

		bool set(const FPPoint &_center, const FPVector &_weight,
		//	 const FPVector _orientation[3],
			 const T _R, const FPVector _rotVector, const T _angle, const T _exponent)
		{
			Point<T>::set(_center, _weight, _R, _rotVector, _angle, _exponent);
			return Point<T>::recomputeOrientation(this->rotVector, this->angle, this->orientation);
		}

		virtual ~Sphere() { }

		virtual bool fromXML(TiXmlHandle &root);

		virtual TiXmlElement * const toXML() const;

		virtual void getBoundingBox(FPPoint &_minCorner,
					    FPPoint &_maxCorner) const
		{ Point<T>::getBoundingBox(_minCorner, _maxCorner); }

		virtual void print(unsigned int indent) const;

		bool empty() const { return false; }

	private:
		FPVector orientation[3];
		virtual T rawValue(const FPPoint &p) const;
};

} // end namespace

#include <shapes/Sphere.hh>

#endif
