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
#include <iostream>

#include <boost/lexical_cast.hpp>

#include <shapes/Sphere.h>
#include <shapes/Tube.h>
#include <shapes/Union.h>
#include <shapes/Difference.h>

namespace shapes
{

template <typename T>
Intersection<T>::~Intersection()
{
	this->clear();
}

template <typename T>
void Intersection<T>::clear()
{
	for (typename std::vector<Structure<T> *>::const_iterator i = structures.begin();
	     i != structures.end(); ++i)
		delete *i;

	structures.clear();
	this->setName("");
}

template <typename T>
void Intersection<T>::add(Structure<T> *structure)
{
	structures.push_back(structure);
}

template <typename T>
T Intersection<T>::rawValue(const FPPoint &p) const
{
	T val = 0.0;

	for (typename std::vector<Structure<T> *>::const_iterator i = structures.begin();
	     i != structures.end(); ++i)
		val += std::pow( (*i)->value(p), -exponent );

	return std::pow( val, T(-1)/exponent );
}

template <typename T>
bool Intersection<T>::fromXML(TiXmlHandle &root)
{
	using boost::lexical_cast;

	this->clear();

	TiXmlElement * elem = root.FirstChildElement("Name").ToElement();
	if (elem)
		this->setName(elem->GetText());

	if ( (elem = root.FirstChildElement("Exponent").ToElement()) )
		this->exponent = lexical_cast<T>(elem->GetText());
	if (!(exponent > T(0)))
	{
		std::cout << "Intersection"
			<< (this->name().empty() ? "" : " \""+this->name()+"\"")
			<< ": Exponent parse failure, Exponent not greater than zero." << std::endl;
		return false;
	}

	if ( (elem = root.FirstChildElement("DampLow").ToElement()) )
		this->dampLow = lexical_cast<T>(elem->GetText());

	if ( (elem = root.FirstChildElement("DampHigh").ToElement()) )
		this->dampHigh = lexical_cast<T>(elem->GetText());

	unsigned int spheres = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Sphere").ToElement();
	     elem; elem = elem->NextSiblingElement("Sphere") )
	{
		++spheres;
		Sphere<T> * sphere = new Sphere<T>();
		TiXmlHandle	handle(elem);
		if (!sphere->fromXML(handle))
		{
			std::cout << "Intersection"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Sphere "
				<< spheres << "." << std::endl;
			return false;
		}

		structures.push_back(sphere);
	}

	unsigned int tubes = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Tube").ToElement();
	     elem; elem = elem->NextSiblingElement("Tube") )
	{
		++tubes;
		Tube<T> * tube = new Tube<T>();
		TiXmlHandle	handle(elem);
		if (!tube->fromXML(handle))
		{
			std::cout << "Intersection"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Tube "
				<< tubes << "." << std::endl;
			return false;
		}

		structures.push_back(tube);
	}

	unsigned int unions = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Union").ToElement();
	     elem; elem = elem->NextSiblingElement("Union") )
	{
		++unions;
		Union<T> * _union = new Union<T>();
		TiXmlHandle	handle(elem);
		if (!_union->fromXML(handle))
		{
			std::cout << "Intersection"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Union "
				<< unions << "." << std::endl;
			return false;
		}

		structures.push_back(_union);
	}

	unsigned int intersections = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Intersection").ToElement();
	     elem; elem = elem->NextSiblingElement("Intersection") )
	{
		++intersections;
		Intersection<T> * _intersection = new Intersection<T>();
		TiXmlHandle	handle(elem);
		if (!_intersection->fromXML(handle))
		{
			std::cout << "Intersection"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Intersection "
				<< intersections << "." << std::endl;
			return false;
		}

		structures.push_back(_intersection);
	}

	unsigned int differences = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Difference").ToElement();
	     elem; elem = elem->NextSiblingElement("Difference") )
	{
		++differences;
		Difference<T> * _difference = new Difference<T>();
		TiXmlHandle	handle(elem);
		if (!_difference->fromXML(handle))
		{
			std::cout << "Intersection"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Difference "
				<< differences << "." << std::endl;
			return false;
		}

		structures.push_back(_difference);
	}

	return true;
}

template <typename T>
TiXmlElement * const Intersection<T>::toXML() const
{
	using boost::lexical_cast;

	TiXmlElement * const root = new TiXmlElement("Intersection");

	std::string buf;

	if (!this->name().empty())
	{
		TiXmlElement * const exponentElem = new TiXmlElement("Name");
		exponentElem->LinkEndChild( new TiXmlText(this->name().c_str()) );
		root->LinkEndChild(exponentElem);
	}

//	if (this->exponent != 2)
//	{
		buf = lexical_cast<std::string>(this->exponent);
		TiXmlElement * const exponentElem = new TiXmlElement("Exponent");
		exponentElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		root->LinkEndChild(exponentElem);
//	}

	if (this->dampLow != 1)
	{
		buf = lexical_cast<std::string>(this->dampLow);
		TiXmlElement * const dampLowElem = new TiXmlElement("DampLow");
		dampLowElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		root->LinkEndChild(dampLowElem);
	}

	if (this->dampHigh != 1)
	{
		buf = lexical_cast<std::string>(this->dampHigh);
		TiXmlElement * const dampHighElem = new TiXmlElement("DampHigh");
		dampHighElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		root->LinkEndChild(dampHighElem);
	}

	for (typename std::vector<Structure<T> *>::const_iterator i = structures.begin();
	     i != structures.end(); ++i)
		root->LinkEndChild( (*i)->toXML() );

	return root;
}

template <typename T>
void Intersection<T>::print(unsigned int indent) const
{
	using boost::lexical_cast;

	Structure<T>::printIndented("<Intersection>", indent);
	std::string buf;
/*
	if (this->exponent != 2)
	{
		buf = "<Exponent>"+lexical_cast<std::string>(this->exponent)+"</Exponent>";
		Structure<T>::printIndented(buf, indent + 1);
	}
*/
	if (this->dampLow != 1)
	{
		buf = "<DampLow>"+lexical_cast<std::string>(this->dampLow)+"</DampLow>";
		Structure<T>::printIndented(buf, indent + 1);
	}

	if (this->dampHigh != 1)
	{
		buf = "<DampHigh>"+lexical_cast<std::string>(this->dampHigh)+"</DampHigh>\n";
		Structure<T>::printIndented(buf, indent + 1);
	}

	if (!this->name().empty())
	{
		buf = "<Name>"+this->name()+"</Name>";
		Structure<T>::printIndented(buf, indent + 1);
	}

	for (typename std::vector<Structure<T> *>::const_iterator i = structures.begin();
	     i != structures.end(); ++i)
		(*i)->print(indent + 1);

	Structure<T>::printIndented("</Intersection>\n", indent);
}

} // end namespace
