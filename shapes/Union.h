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

#ifndef SHAPES_UNION_H
#define SHAPES_UNION_H 1

#include <vector>
#include <shapes/Structure.h>

namespace shapes
{

template <typename T>
class Union : public Structure<T>
{
	public:
		typedef typename Structure<T>::FPPoint  FPPoint;
		typedef typename Structure<T>::FPVector FPVector;

		Union(const std::string name__ = "", T _exponent = 2.) :
			Structure<T>(name__), exponent(_exponent) { }

		Union(T _exponent, const std::string name__ = "") :
			Structure<T>(name__), exponent(_exponent) { }

		virtual ~Union();

		virtual bool fromXML(TiXmlHandle &root);

		virtual TiXmlElement * const toXML() const;

		virtual void getBoundingBox(FPVector &_minCorner,
					    FPVector &_maxCorner) const
		{
			_minCorner =  std::numeric_limits<T>::max();
			_maxCorner = -std::numeric_limits<T>::max();

			for (typename std::vector<Structure<T> *>::
			     const_iterator i = structures.begin();
			     i != structures.end(); ++i)
			{
				FPVector minCorner, maxCorner;

				(*i)->getBoundingBox(minCorner, maxCorner);
				_minCorner.x() = std::min(_minCorner.x(), minCorner.x());
				_minCorner.y() = std::min(_minCorner.y(), minCorner.y());
				_minCorner.z() = std::min(_minCorner.z(), minCorner.z());

				_maxCorner.x() = std::max(_maxCorner.x(), maxCorner.x());
				_maxCorner.y() = std::max(_maxCorner.y(), maxCorner.y());
				_maxCorner.z() = std::max(_maxCorner.z(), maxCorner.z());
			}
		}

		virtual void print(unsigned int indent) const;

		void add(Structure<T> *structure);

		void clear();

		bool empty() const { return structures.empty(); }

	private:
		virtual T rawValue(const FPPoint &p) const;
		std::vector<Structure<T> *> structures;
		T exponent;
};

} // end namespace

#include <shapes/Union.hh>

#endif
