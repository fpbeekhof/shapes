/***************************************************************************
 *   Copyright (C) 2006-2009 by F. P. Beekhof                              *
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

#ifndef SHAPES_POINT_H
#define SHAPES_POINT_H 1

#include <shapes/tinyxml.h>
#include <shapes/EuclidTypes.h>

namespace shapes
{

template <typename T>
class Point
{
	public:
		typedef typename EuclidTypes<T>::FPPoint  FPPoint;
		typedef typename EuclidTypes<T>::FPVector FPVector;

		Point() : center(0.0), weight(1.0), R(0.0),
				angle(0.0), exponent(2.0)
		{
			rotVector[X] = 1.0;
			rotVector[Y] = rotVector[Z] = 0.;
/*
			// Three unit vectors
			for (unsigned i = 0; i < 3; ++i)
			{
				orientation[i]    = 0.0;
				orientation[i][i] = 1.0;
			}
*/
		}

		Point(const FPPoint &_center,
			const FPVector &_weight,
//			const FPVector _orientation[3],
			const T _R,
			const FPVector _rotVector, const T _angle, // Radians!!
			const T _exponent = 2.0);

		bool set(const FPPoint &_center,
			 const FPVector &_weight,
//			 const FPVector _orientation[3],
			 const T _R,
			 const FPVector _rotVector, const T _angle,  // Radians!!
			 const T _exponent = 2.0);

		FPVector getCenter() const { return center; }

		T getCenter(unsigned dim) const { return center[dim]; }

		FPVector getWeight() const { return weight; }

		T getWeight(unsigned dim) const { return weight[dim]; }
/*
		const FPVector &getOrientation(unsigned i) const
		{ return orientation[i]; }

		T getOrientation(unsigned i, unsigned dim) const
		{ return orientation[i][dim]; }
*/
		const FPVector &getRotationVector() const { return rotVector; }

		bool setRotationVector(const FPVector &direction)
		{
			rotVector = direction;
			rotVector /= cvmlcpp::modulus(rotVector); // Normalize to unit vector
			return recomputeOrientation();
		}

		T getAngle() const { return angle; } // in RADIALS

		void setAngle(const T radians)
		{
			angle = radians;
			recomputeOrientation();
		}

		T getRadius() const { return R; }

		T getExponent() const { return exponent; }

		bool fromXml(TiXmlHandle &root);

		TiXmlElement * const toXML(const std::string rootTag) const;

		void printIndented(unsigned indent) const;

		void getBoundingBox(FPPoint &_minCorner,
				    FPPoint &_maxCorner) const
		{
			assert(this->R >= 0.);
			_minCorner =  std::numeric_limits<T>::max();
			_maxCorner = -std::numeric_limits<T>::max();
			for (int i = 0; i < 3; ++i)
			{
				_minCorner[i] = std::min(_minCorner[i], this->center[i] - this->R / weight[i]);
				_maxCorner[i] = std::max(_maxCorner[i], this->center[i] + this->R / weight[i]);
			}
		}

		static bool recomputeOrientation(const FPVector &rv,
					const T a, FPVector orientation[3])
		{
			// Use negative angle, orientation will be used to
			// rotate sample positions rather than the structure

			// Rodrigues
			// http://en.wikipedia.org/wiki/Rotation_vector
			// http://en.wikipedia.org/wiki/Cross_product
			// http://mathworld.wolfram.com/RodriguesRotationFormula.html
			// http://en.wikipedia.org/wiki/Rodrigues'_rotation_formula

			const T c = std::cos(-a);
			const T c1 = (T(1) - c);
			const T s = std::sin(-a);

			const T x = rv[X];
			const T y = rv[Y];
			const T z = rv[Z];
			const T x2 = rv[X]*rv[X];
			const T y2 = rv[Y]*rv[Y];
			const T z2 = rv[Z]*rv[Z];

			// W2 = W'*W
//			orientation[X][X] = 1 + (y2+z2)*c1;
			orientation[X][X] = c + x2*c1;
			orientation[X][Y] = -z*s - x*y*c1;
			orientation[X][Z] =  y*s - x*z*c1;

			orientation[Y][X] =  z*s - x*y*c1;
//			orientation[Y][Y] = 1 + (x2+z2)*c1;
			orientation[Y][Y] = c + y2*c1;
			orientation[Y][Z] = -x*s - y*z*c1;

			orientation[Z][X] = -y*s - x*z*c1;
			orientation[Z][Y] =  x*s - y*z*c1;
//			orientation[Z][Z] = 1 + (x2+y2)*c1;
			orientation[Z][Z] = c + z2*c1;

			for (int i = 0; i < 3; ++i)
			{
				const T length = cvmlcpp::modulus(orientation[i]);
				if (! (length > 0) )
					return false;
				orientation[i] /= length;
			}

			return true;
		}

	protected:
		FPPoint  center;
		FPVector weight;
		FPVector rotVector;
		T	 R, angle, exponent;
};

} // end namespace

#include <shapes/Point.hh>

#endif
