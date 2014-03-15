/***************************************************************************
 *   Copyright (C) 2011 by F. P. Beekhof                                   *
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

#ifndef SHAPES_SPHERIC_STRUCTURE_H
#define SHAPES_SPHERIC_STRUCTURE_H 1

#include <shapes/Structure.h>

namespace shapes
{

template <typename T>
class SphericStructure : public Structure<T>
{
	public:
		typedef typename EuclidTypes<T>::FPPoint  FPPoint;
		typedef typename EuclidTypes<T>::FPVector FPVector;

		SphericStructure(const std::string name__ = "") : Structure<T>(name__) { }

		SphericStructure(const T dampLow__, const T dampHigh__, const std::string name__ = "") :
			Structure<T>(dampLow__, dampHigh__, name__) { }

		virtual ~SphericStructure() { }

	protected:
		T sphereValue(const T distSq, const T e, const T r) const
		{
			T val_inv = std::pow( distSq/(r*r), e * T(0.5));

			// Capping value, to avoid infty
			const T gamma = std::numeric_limits<T>::max() / 2.;

			// delta chosen to be a number small yet still significant with
			// respect to gamma.
			const T delta = gamma / std::pow(T(2), T(std::numeric_limits<T>::digits));
			assert(gamma + delta > gamma);
			assert(gamma - delta < gamma);

			// We do the case distinction here instead of in s to avoid computing 1/0
			// Because val_inv = 1/f in report eqn. (32), we must invert the inequalities
			return   (val_inv >= 1/(gamma-delta)) ?   1/val_inv :
				((val_inv >= 1/(gamma+delta)) ? s(1/val_inv, gamma, delta) : gamma);
		}

		static T s(const T x, const T gamma, const T delta)
		{
			// only the middle case of eq. (32) in the report
			return x -  2*delta*S( (x - gamma+delta)/(2*delta) );
		}
		static T S(const T x)
		{
			return x*x*x*(1-x/2);
		}
};

} // end namespace

#endif
