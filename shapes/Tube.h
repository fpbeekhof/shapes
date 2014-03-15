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

#ifndef SHAPES_TUBE_H
#define SHAPES_TUBE_H 1

#include <vector>

#include <cvmlcpp/base/Enums>
#include <cvmlcpp/math/Polynomial>
#include <cvmlcpp/math/Splines>

#include <shapes/EuclidTypes.h>
#include <shapes/SphericStructure.h>
#include <shapes/Point.h>

#ifdef USE_GSL
#include <gsl/gsl_errno.h>
#include <gsl/gsl_poly.h>
#endif

namespace shapes
{

template <typename T>
class Tube : public SphericStructure<T>
{
	public:
		typedef typename Structure<T>::FPPoint  FPPoint;
		typedef typename Structure<T>::FPVector FPVector;

		Tube(const std::string name__ = "") : SphericStructure<T>(name__)
		{
			#ifdef USE_GSL
			gsl_set_error_handler_off();
			#endif
		}

		Tube(const std::vector<Point<T> > &_points, const std::string name__ = "") :
			SphericStructure<T>(name__), points(_points)
		{
			if (!this->generateTubes(points))
				std::cout << "Tube: Generation of splines failed." << std::endl;

			#ifdef USE_GSL
			gsl_set_error_handler_off();
			#endif
		}

		bool set(const std::vector<Point<T> > &_points)
		{
			points = _points;
			return this->generateTubes(points);
		}

		const std::vector<Point<T> > &getPoints() const { return points; }

		virtual ~Tube() { }

		virtual bool fromXML(TiXmlHandle &root);

		virtual TiXmlElement * const toXML() const;

		virtual void getBoundingBox(FPPoint &_minCorner,
					    FPPoint &_maxCorner) const
		{
			_minCorner =  std::numeric_limits<T>::max();
			_maxCorner = -std::numeric_limits<T>::max();

			for (typename std::vector<Point<T> >::const_iterator p = points.begin();
			     p != points.end(); ++p)
			{
				FPVector minCorner, maxCorner;
				p->getBoundingBox(minCorner, maxCorner);

				for (int i = 0; i < 3; ++i)
				{
					_minCorner[i] = std::min(_minCorner[i], minCorner[i]);
					_maxCorner[i] = std::max(_maxCorner[i], maxCorner[i]);
				}
			}
		}

		virtual void print(unsigned indent) const;

                bool empty() const { return points.empty(); }


	private:
		virtual T rawValue(const FPPoint &p) const;

		std::vector<Point<T> >		points;

		cvmlcpp::NaturalCubicSpline<FPPoint,  3> center;
		cvmlcpp::NaturalCubicSpline<FPVector, 3> weight;
		cvmlcpp::NaturalCubicSpline<FPVector, 3> rotVector;
		cvmlcpp::NaturalCubicSpline<T, 1> angle;
//		cvmlcpp::NaturalCubicSpline<FPVector, 3> orientation[3];
		cvmlcpp::NaturalCubicSpline<T, 1>	 exponent;
		cvmlcpp::NaturalCubicSpline<T, 1>	 radius;
//		mutable T 			cachedT;

		bool generateTubes(const std::vector<Point<T> > &points);

//		void getDerivative(const T &t, FPVector &v) const;
		cvmlcpp::Polynomial<T, 6>
		distSqPoly(const std::size_t segment, const FPPoint &p) const;

		T segmentValue(const std::size_t segment, const FPPoint &p) const;

		bool updateMinDistSq(const std::size_t segment, const FPPoint &p,
					const T &t, T &minDistSq, T &best) const;

		bool findTSegment(const std::size_t segment, const FPPoint &p, T &t) const;


		bool findTSegment_NewtonRaphson(const std::size_t segment,
			const FPPoint &p, T &t,
			const cvmlcpp::Polynomial<T, 5> &derivativeDistSq) const;

};

} // end namespace

#include <shapes/Tube.hh>

#endif
