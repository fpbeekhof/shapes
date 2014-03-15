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

#include <cassert>
#include <cmath>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include <cvmlcpp/base/StringTools>

#include "Point.h"

namespace shapes
{

template <typename T>
cvmlcpp::Polynomial<T, 6>
Tube<T>::distSqPoly(const std::size_t segment, const FPPoint &p) const
{
	cvmlcpp::Polynomial<T, 6> distSq;
	std::fill(distSq.begin(), distSq.end(), 0);

	const cvmlcpp::Polynomial<FPPoint, 3> d = center[segment] - p;
	cvmlcpp::Polynomial<FPPoint, 6> dSq = d * d;
	for (unsigned t = 0u; t <= 6u; ++t)
			distSq[t] = dSq[t][X] + dSq[t][Y] + dSq[t][Z];

	return distSq;
}

template <typename T>
T Tube<T>::rawValue(const FPPoint &p) const
{
	assert(weight.size() == center.size());
/*	assert(orientation[X].size() == center.size());
	assert(orientation[Y].size() == center.size());
	assert(orientation[Z].size() == center.size());
*/	assert(rotVector.size() == center.size());
	assert(angle.size() == center.size());
	assert(exponent.size() == center.size());
	assert(radius.size() == center.size());

	// Of each tube, the maximum of the contributions of the
	// segments is taken as contribution of the tube to the point.
	T val = -1.0;
	for (std::size_t segment = 0; segment < center.size(); ++segment)
		val = std::max(val, this->segmentValue(segment, p));

	assert( (points.size() == 0u) || (val >= 0.0) );

	return val;
}

template <typename T>
T Tube<T>::segmentValue(const std::size_t segment, const FPPoint &p) const
{
	T t;
	if (!this->findTSegment(segment, p, t)) // No valid closest point
		return T(0);

	assert(t >= 0.0);
	assert(t <= 1.0);

	// Splines<T> require the segment number to be included
	t += segment;

	// Get the normalized weights
	const FPVector weightt = weight(t);
	assert( std::abs(cvmlcpp::modulus(weightt)-std::sqrt(T(3))) < 0.00001 );
	assert(weightt[X] > 0.);
	assert(weightt[Y] > 0.);
	assert(weightt[Z] > 0.);

	// Compute point on axis 'c', and vector c-p
	const FPVector cp = this->center(t) - p;

	const T a = this->angle(t);
	FPVector rv = this->rotVector(t);
	assert(std::abs(cvmlcpp::modulus(rv)-1) < 0.00001);
	FPVector orientation[3];
	Point<T>::recomputeOrientation(rv, a, orientation);

	assert(std::abs(cvmlcpp::modulus(orientation[X])-1) < 0.00001);
	assert(std::abs(cvmlcpp::modulus(orientation[Y])-1) < 0.00001);
	assert(std::abs(cvmlcpp::modulus(orientation[Z])-1) < 0.00001);

	assert(this->exponent(t) > 0.0);
	assert(this->radius(t) > 0);

	const T x = cvmlcpp::dotProduct(cp, orientation[X]) / (weightt[X]);
	const T y = cvmlcpp::dotProduct(cp, orientation[Y]) / (weightt[Y]);
	const T z = cvmlcpp::dotProduct(cp, orientation[Z]) / (weightt[Z]);

	const T distSq = x*x+y*y+z*z;

	return this->sphereValue(distSq, this->exponent(t), this->radius(t));
}

template <typename T>
bool Tube<T>::fromXML(TiXmlHandle &root)
{
	using boost::lexical_cast;

	points.clear();

	this->setName("");
	TiXmlElement * elem = root.FirstChildElement("Name").ToElement();
	if (elem)
		this->setName(elem->GetText());

	if ( (elem = root.FirstChildElement("DampLow").ToElement()) )
		this->dampLow = lexical_cast<T>(elem->GetText());

	if ( (elem = root.FirstChildElement("DampHigh").ToElement()) )
		this->dampHigh = lexical_cast<T>(elem->GetText());

	for (TiXmlElement * elem = root.FirstChildElement("Point").ToElement();
	     elem; elem = elem->NextSiblingElement("Point") )
	{
		Point<T>	point;
		TiXmlHandle	handle(elem);
		if (!point.fromXml(handle))
		{
			std::cout << "Tube "
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Point " << (points.size() + 1) << "." << std::endl;
			return false;
		}

		points.push_back(point);
	}

	if (points.size() < 3)
	{
		std::cout << "Tube: insufficient points." << std::endl;
		return false;
	}

	if (!this->generateTubes(points))
	{
		std::cout << "Tube: Generation of splines failed." << std::endl;
		return false;
	}

	return true;
}

template <typename T>
TiXmlElement * const Tube<T>::toXML() const
{
	using boost::lexical_cast;

	TiXmlElement * const root = new TiXmlElement("Tube");

	if (!this->name().empty())
	{
		TiXmlElement * const exponentElem = new TiXmlElement("Name");
		exponentElem->LinkEndChild( new TiXmlText(this->name().c_str()) );
		root->LinkEndChild(exponentElem);
	}

	if (this->dampLow != 1)
	{
		std::string buf = lexical_cast<std::string>(this->dampLow);
		TiXmlElement * const dampLowElem = new TiXmlElement("DampLow");
		dampLowElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		root->LinkEndChild(dampLowElem);
	}

	if (this->dampHigh != 1)
	{
		std::string buf = lexical_cast<std::string>(this->dampHigh);
		TiXmlElement * const dampHighElem = new TiXmlElement("DampHigh");
		dampHighElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		root->LinkEndChild(dampHighElem);
	}

	for (typename std::vector<Point<T> >::const_iterator p = points.begin();
	     p != points.end(); ++p)
		root->LinkEndChild(p->toXML("Point"));

	return root;
}

template <typename T>
void Tube<T>::print(unsigned indent) const
{
	Structure<T>::printIndented("<Tube>", indent);

	std::string buf;
	if (this->dampLow != 1)
	{
		buf = "<DampLow>"+boost::lexical_cast<std::string>(this->dampLow)+"</DampLow>";
		Structure<T>::printIndented(buf, indent + 1);
	}

	if (this->dampHigh != 1)
	{
		buf = "<DampHigh>"+boost::lexical_cast<std::string>(this->dampHigh)+"</DampHigh>";
		Structure<T>::printIndented(buf, indent + 1);
	}

	if (!this->name().empty())
	{
		buf = "<Name>"+this->name()+"</Name>";
		Structure<T>::printIndented(buf, indent + 1);
	}

	for (typename std::vector<Point<T> >::const_iterator it = points.begin();
	     it != points.end(); ++it)
	{
		for (unsigned i = 0; i <= indent; ++i)
			std::cout << "\t";
		std::cout << "<Point>" << std::endl;
		it->printIndented(indent+1);
		for (unsigned i = 0; i <= indent; ++i)
			std::cout << "\t";
		std::cout << "</Point>" << std::endl;
	}

	Structure<T>::printIndented("</Tube>\n", indent);
}


template <typename T>
bool Tube<T>::generateTubes(const std::vector<Point<T> > &points)
{
	if (points.size() < 3)
		return false;

	// Center
	std::vector<FPPoint> pts;
	pts.clear();
	for (typename std::vector<Point<T> >::const_iterator point = points.begin();
	     point != points.end(); point++)
		pts.push_back(point->getCenter());
	center.init(pts.begin(), pts.end());

	// Weight
	pts.clear();
	for (typename std::vector<Point<T> >::const_iterator point = points.begin();
	     point != points.end(); ++point)
		pts.push_back(point->getWeight());
	weight.init(pts.begin(), pts.end());

	// Orientation
	pts.clear();
	for (typename std::vector<Point<T> >::const_iterator point = points.begin();
	     point != points.end(); ++point)
		pts.push_back(point->getRotationVector());
	rotVector.init(pts.begin(), pts.end());

	// Angle
	std::vector<T> values;
	for (typename std::vector<Point<T> >::const_iterator point = points.begin();
	     point != points.end(); ++point)
		values.push_back(point->getAngle());
	angle.init(values.begin(), values.end());

/*
	// Orientation in 3 x 3 dimensions
	for (unsigned vec = 0; vec < 3; ++vec)
	{
		pts.clear();
		for (typename std::vector<Point<T> >::const_iterator point = points.begin();
		     point != points.end(); ++point)
			pts.push_back(point->getOrientation(vec));

		orientation[vec].init(pts.begin(), pts.end());
	}
*/
	// Radius
	values.clear();
	for (typename std::vector<Point<T> >::const_iterator point = points.begin();
	     point != points.end(); ++point)
	{
		assert(point->getRadius() > 0.0);
		values.push_back(point->getRadius());
	}
	radius.init(values.begin(), values.end());

	// Exponent
	values.clear();
	for (typename std::vector<Point<T> >::const_iterator point = points.begin();
	     point != points.end(); ++point)
		values.push_back(point->getExponent());
	exponent.init(values.begin(), values.end());

	return true;
}

template <typename T>
bool Tube<T>::updateMinDistSq(const std::size_t segment, const FPPoint &p,
		const T &t, T &minDistSq, T &best) const
{
	assert(t >= 0.0); // May be equal during search for t
	assert(t <= 1.0);

	// find vector from point to center(i)
	FPVector cp = p - this->center(T(segment)+t);
	const T distSq = cvmlcpp::dotProduct(cp, cp);

	// Closer to axis than best closest point sofar ?
	if (distSq < minDistSq)
	{
		minDistSq = distSq;
		best = t;
		return true;
	}

	// No improvement
	return false;
}

/*
 * The following section is dedicated to finding the parameter 't' that
 * yields the shortest distance from a segment of a tube (a cubic spline)
 * to the point 'p'.
 * Calculating the distance requires taking the square root of a 6th
 * degree polynomial; minimizing the distance implies minimizing the
 * the 6th degree polynomial. To do this, we need to find the roots of
 * its derivative, itself a 5th degree polynomial. To do this we either
 * a) use gsl; or b) use a newton-raphson procedure with intelligently
 * chosen starting point.
 * Using gsl is preffered.
 */
#ifdef USE_GSL
template <typename T>
bool Tube<T>::findTSegment(const std::size_t segment, const FPPoint &p, T &t) const
{
	// Create the derivative of the distance to p - a 5th degree Polynomial3
	const cvmlcpp::Polynomial<T, 5> derivative1DistSq =
		this->distSqPoly(segment, p).derivative();

	int terms = 6;
	while ( (terms > 0) && (derivative1DistSq[terms-1] == 0) )
		--terms;

	gsl_poly_complex_workspace *w = gsl_poly_complex_workspace_alloc(terms);
	if (w == NULL) // If somehow the workspace wasn't created...
	{
		// ...fall back to newton-raphson
		return findTSegment_NewtonRaphson(segment, p, t, derivative1DistSq);
	}
	assert(w != NULL);

	// Try to evaluate
	double a[6];
	std::copy(derivative1DistSq.begin(), derivative1DistSq.end(), a);
	double z[10];

	const int result = gsl_poly_complex_solve(a, terms, w, z);
	gsl_poly_complex_workspace_free (w);
	if (result != GSL_SUCCESS) // If root-finding doesn't converge...
	{
		// ...fall back to newton-raphson
		return findTSegment_NewtonRaphson(segment, p, t, derivative1DistSq);
	}

	// Initial invalid flag value
	t = -1.0;
	// Find best of valid solutions...
	T minDistSq = std::numeric_limits<T>::max();
	for (int i = 0; i < terms-1; ++i)
	{
		const T real = z[2*i];
		const T imag = z[2*i+1];
		if ( (real >= 0.0) && (real <= 1.0) && // In range?
		     (std::abs(imag) < 1e-20) ) // Not complex?
			updateMinDistSq(segment, p, real, minDistSq, t); // Improvement?
	}
	// ...and try endpoints too.
	updateMinDistSq(segment, p, 0.0, minDistSq, t); // Improvement?
	updateMinDistSq(segment, p, 1.0, minDistSq, t); // Improvement?

	return t != -1.0;
}
/*
template <>
bool Tube<double>::findTSegment(const FPPoint &p, double &t) const
{

}
*/
#else
// No gsl, pass directly to Newton-Raphson
template <typename T>
bool Tube<T>::findTSegment(const std::size_t segment, const FPPoint &p, T &t) const
{
	return findTSegment_NewtonRaphson(segment, p, t,
		this->distSqPoly(segment, p).derivative());
}
#endif

/*
 * This entire section is required only if gsl is not available or if
 * gsl fails to find the roots.
 */

template <typename T>
bool Tube<T>::findTSegment_NewtonRaphson(const std::size_t segment, const FPPoint &p, T &t,
				const cvmlcpp::Polynomial<T, 5> &derivative1DistSq) const
{
	const unsigned iterations = 128;
	const T verySmall = 0.0000001;

	cvmlcpp::Polynomial<T, 4> derivative2DistSq = derivative1DistSq.derivative();
	cvmlcpp::Polynomial<T, 3> derivative3DistSq = derivative2DistSq.derivative();

	T minDistSq = std::numeric_limits<T>::max();

	t = -1.0;

/*
	// Try cached result
	if (cachedT >= 0.0 && cachedT <= 1.0)
	{
		T guess = cachedT;
		if ( doNewtonRaphson(derivative1DistSq, derivative2DistSq,
			    guess, 0.0 , 1.0, iterations) &&
		     (updateMinDistSq(segment, p, guess, minDistSq)) )
			cachedT = t = guess;
		tried.push_back(guess);
	}
*/
	// From now on, do not determine distance if close to cachedT

	// Try t = 0
	T t0 = 0.0;
	if ( cvmlcpp::doNewtonRaphson(derivative1DistSq, derivative2DistSq,
			     t0, 0.0 , 1.0, iterations) )
		updateMinDistSq(segment, p, t0, minDistSq, t);

	// Try t = 1
	T t1 = 1.0;
	if ( doNewtonRaphson(derivative1DistSq, derivative2DistSq,
			     t1, 0.0 , 1.0, iterations) )
		updateMinDistSq(segment, p, t1, minDistSq, t);

	// Try zero-points of 2nd degree taylor-approximation of
	// derative of distSq as starting points.
	// I.e. Solve f(x) = d1 + d2*x + d3/2 * x^2 == 0
	// The function is centered around t=0.5, so t= x+0.5
	const T d1 = derivative1DistSq(0.5);
	const T d2 = derivative2DistSq(0.5);
	const T d3 = derivative3DistSq(0.5);

	const T disc = d2*d2 - 2.0 * d3 * d1;
// cout << endl << "Disc " << disc << " d1 " << d1 << " d2 " << d2 << " d3 "
// 	<< d3 << endl;

	if ( (disc >= 0.0) && (std::abs(d3) > verySmall) )
	{
		T guessA = 0.5 - (d2 - sqrt(disc)) / d3;
		if (guessA >= 0.0 && guessA <= 1.0)
		{
			if ( doNewtonRaphson(derivative1DistSq, derivative2DistSq,
						guessA, 0.0 , 1.0, iterations) )
				updateMinDistSq(segment, p, guessA, minDistSq, t);
		}

		T guessB = 0.5 - (d2 + sqrt(disc)) / d3;
		if (guessB >= 0.0 && guessB <= 1.0)
		{
			if ( doNewtonRaphson(derivative1DistSq, derivative2DistSq,
						guessB, 0.0 , 1.0, iterations) )
				updateMinDistSq(segment, p, guessB, minDistSq, t);
		}
	}
	else if (std::abs(d2) > verySmall)
	{
		// If d3 = 0; then it is a linear function, not 2nd d
		//  so we solve f(x) = d1 + d2*x == 0
		T guess = 0.5 - d1 / d2;
		if (guess >= 0.0 && guess <= 1.0)
		{
			if ( doNewtonRaphson(derivative1DistSq, derivative2DistSq,
						guess, 0.0 , 1.0, iterations) )
				updateMinDistSq(segment, p, guess, minDistSq, t);
		}
	}

	assert( (t == -1.0) || (t >= 0.0 && t <= 1.0) );

	return (t >= 0.0 && t <= 1.0);
}

} // end namespace
