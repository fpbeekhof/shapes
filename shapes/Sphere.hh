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

namespace shapes
{

template <typename T>
bool Sphere<T>::fromXML(TiXmlHandle &root)
{
	using boost::lexical_cast;

	this->setName("");
	TiXmlElement * elem = root.FirstChildElement("Name").ToElement();
	if (elem)
		this->setName(elem->GetText());

	if ( (elem = root.FirstChildElement("DampLow").ToElement()) )
		this->dampLow = lexical_cast<T>(elem->GetText());

	if ( (elem = root.FirstChildElement("DampHigh").ToElement()) )
		this->dampHigh = lexical_cast<T>(elem->GetText());

	if (!this->fromXml(root))
	{
		std::cout << "Sphere"
			<< (this->name().empty() ? "" : " \""+this->name()+"\"")
			<< ": point parse failure." << std::endl;
		return false;
	}
	if (!Point<T>::recomputeOrientation(this->rotVector, this->angle, orientation))
	{
		std::cout << "Sphere"
			<< (this->name().empty() ? "" : " \""+this->name()+"\"")
			<< ": rotation vector problem." << std::endl;
		return false;
	}

	return true;
}

template <typename T>
TiXmlElement * const Sphere<T>::toXML() const
{
	TiXmlElement * const root = Point<T>::toXML("Sphere");

	using boost::lexical_cast;

	if (!this->name().empty())
	{
		TiXmlElement * const exponentElem = new TiXmlElement("Name");
		exponentElem->LinkEndChild( new TiXmlText(this->name().c_str()) );
		root->LinkEndChild(exponentElem);
	}

//	std::string buf;
/*
	buf = lexical_cast<std::string>(this->center.x()) + " " +
		lexical_cast<std::string>(this->center.y()) + " " +
		lexical_cast<std::string>(this->center.z());
	TiXmlElement * const centerElem = new TiXmlElement("Center");
	centerElem->LinkEndChild( new TiXmlText(buf.c_str()) );
	root->LinkEndChild(centerElem);

	buf = lexical_cast<std::string>(this->weight.x()) + " " +
		lexical_cast<std::string>(this->weight.y()) + " " +
		lexical_cast<std::string>(this->weight.z());
	TiXmlElement * const weightElem = new TiXmlElement("Weight");
	weightElem->LinkEndChild( new TiXmlText(buf.c_str()) );
	root->LinkEndChild(weightElem);

	TiXmlElement * const orientationElem = new TiXmlElement("Orientation");
	for (unsigned int i = 0; i < 3; ++i)
	{
		buf = lexical_cast<std::string>(this->orientation[i].x()) + " " +
			lexical_cast<std::string>(this->orientation[i].y()) + " " +
			lexical_cast<std::string>(this->orientation[i].z());
		TiXmlElement * const axisElem = new TiXmlElement("Axis");
		axisElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		orientationElem->LinkEndChild( axisElem );
	}

	buf = lexical_cast<std::string>(this->R);
	TiXmlElement * const radiusElem = new TiXmlElement("Radius");
	radiusElem->LinkEndChild( new TiXmlText(buf.c_str()) );
	root->LinkEndChild(radiusElem);

	buf = lexical_cast<std::string>(this->exponent);
	TiXmlElement * const exponentElem = new TiXmlElement("Exponent");
	exponentElem->LinkEndChild( new TiXmlText(buf.c_str()) );
	root->LinkEndChild(exponentElem);
*/

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

	return root;
}

template <typename T>
T Sphere<T>::rawValue(const FPPoint &p) const
{
	const FPVector cp = p - this->center;

	assert(std::abs(std::sqrt(T(3))- cvmlcpp::modulus(this->weight)) < T(0.001));

	assert(std::abs(cvmlcpp::modulus(this->orientation[X])-1) < 0.00001);
	assert(std::abs(cvmlcpp::modulus(this->orientation[Y])-1) < 0.00001);
	assert(std::abs(cvmlcpp::modulus(this->orientation[Z])-1) < 0.00001);

/*
	const T x = std::abs( cvmlcpp::dotProduct(cp, this->orientation[X] / this->weight[X]) );
	const T y = std::abs( cvmlcpp::dotProduct(cp, this->orientation[Y] / this->weight[Y]) );
	const T z = std::abs( cvmlcpp::dotProduct(cp, this->orientation[Z] / this->weight[Z]) );
*/

	assert(this->R > 0);
	assert(this->exponent > 0);

	const T x = cvmlcpp::dotProduct(cp, this->orientation[X]) / (this->weight[X]);
	const T y = cvmlcpp::dotProduct(cp, this->orientation[Y]) / (this->weight[Y]);
	const T z = cvmlcpp::dotProduct(cp, this->orientation[Z]) / (this->weight[Z]);
	const T distSq = x*x+y*y+z*z;

	return this->sphereValue(distSq, this->exponent, this->R);
}

template <typename T>
void Sphere<T>::print(unsigned int indent) const
{
	using boost::lexical_cast;

	Structure<T>::printIndented("<Sphere>", indent);

	std::string buf;

	// DampLow
	if (this->dampLow != 1)
	{
		buf = "<DampLow>"+lexical_cast<std::string>(this->dampLow)+"</DampLow>";
		Structure<T>::printIndented(buf, indent + 1);
	}

	// DampHigh
	if (this->dampLow != 1)
	{
		buf = "<DampHigh>"+lexical_cast<std::string>(this->dampHigh)+"</DampHigh>";
		Structure<T>::printIndented(buf, indent + 1);
	}

	if (!this->name().empty())
	{
		buf = "<Name>"+this->name()+"</Name>";
		Structure<T>::printIndented(buf, indent + 1);
	}

	Point<T>::printIndented(indent);

	Structure<T>::printIndented("</Sphere>\n", indent);
}

} // end namespace

