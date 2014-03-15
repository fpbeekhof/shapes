/***************************************************************************
 *   Copyright (C) 2009 by F. P. Beekhof                                   *
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
#include <shapes/Intersection.h>

namespace shapes
{

template <typename T>
Difference<T>::~Difference()
{
	this->clear();
}

template <typename T>
void Difference<T>::clear()
{
	for (typename std::vector<Structure<T> *>::const_iterator
	     i = positiveStructures.begin(); i != positiveStructures.end(); ++i)
		delete *i;

	for (typename std::vector<Structure<T> *>::const_iterator
	     i = negativeStructures.begin(); i != negativeStructures.end(); ++i)
		delete *i;

	positiveStructures.clear();
	negativeStructures.clear();
	this->setName("");
}

template <typename T>
void Difference<T>::addPositive(Structure<T> *structure)
{
	positiveStructures.push_back(structure);
}

template <typename T>
void Difference<T>::addNegative(Structure<T> *structure)
{
	negativeStructures.push_back(structure);
}

template <typename T>
T Difference<T>::rawValue(const FPPoint &p) const
{
	T val = 0.0;

	for (typename std::vector<Structure<T> *>::const_iterator
	     i = positiveStructures.begin();
	     i != positiveStructures.end(); ++i)
		val += pow( (*i)->value(p), -exponent );

	for (typename std::vector<Structure<T> *>::const_iterator
	     i = negativeStructures.begin();
	     i != negativeStructures.end(); ++i)
		val += pow( (*i)->value(p), exponent );

	return pow(val, T(-1)/exponent );
}

template <typename T>
unsigned Difference<T>::sectionFromXML(TiXmlHandle root, std::vector<Structure<T> *> &structures)
{

	unsigned spheres = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Sphere").ToElement();
	     elem; elem = elem->NextSiblingElement("Sphere") )
	{
		++spheres;
		Sphere<T> * sphere = new Sphere<T>();
		TiXmlHandle	handle(elem);
		if (!sphere->fromXML(handle))
		{
			std::cout << "Difference"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Sphere "
				<< spheres << "." << std::endl;
			return false;
		}

		structures.push_back(sphere);
	}

	unsigned tubes = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Tube").ToElement();
	     elem; elem = elem->NextSiblingElement("Tube") )
	{
		++tubes;
		Tube<T> * tube = new Tube<T>();
		TiXmlHandle	handle(elem);
		if (!tube->fromXML(handle))
		{
			std::cout << "Difference"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Tube "
				<< tubes << "." << std::endl;
			return false;
		}

		structures.push_back(tube);
	}

	unsigned unions = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Union").ToElement();
	     elem; elem = elem->NextSiblingElement("Union") )
	{
		++unions;
		Union<T> * _union = new Union<T>();
		TiXmlHandle	handle(elem);
		if (!_union->fromXML(handle))
		{
			std::cout << "Difference"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Union "
				<< unions << "." << std::endl;
			return false;
		}

		structures.push_back(_union);
	}

	unsigned intersections = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Intersection").ToElement();
	     elem; elem = elem->NextSiblingElement("Intersection") )
	{
		++intersections;
		Intersection<T> * _intersection = new Intersection<T>();
		TiXmlHandle	handle(elem);
		if (!_intersection->fromXML(handle))
		{
			std::cout << "Difference"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Intersection "
				<< intersections << "." << std::endl;
			return false;
		}

		structures.push_back(_intersection);
	}

	unsigned differences = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Difference").ToElement();
	     elem; elem = elem->NextSiblingElement("Difference") )
	{
		++differences;
		Difference<T> * _difference = new Difference<T>();
		TiXmlHandle	handle(elem);
		if (!_difference->fromXML(handle))
		{
			std::cout << "Difference"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": parse failure Difference "
				<< differences << "." << std::endl;
			return false;
		}

		structures.push_back(_difference);
	}

	return spheres+tubes+unions+intersections+differences;
}

template <typename T>
bool Difference<T>::fromXML(TiXmlHandle &root)
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
		std::cout << "Difference"
			<< (this->name().empty() ? "" : " \""+this->name()+"\"")
			<< ": Exponent parse failure, Exponent not greater than zero." << std::endl;
		return false;
	}

	if ( (elem = root.FirstChildElement("DampLow").ToElement()) )
		this->dampLow = lexical_cast<T>(elem->GetText());

	if ( (elem = root.FirstChildElement("DampHigh").ToElement()) )
		this->dampHigh = lexical_cast<T>(elem->GetText());

	elem = root.FirstChildElement("Positive").ToElement();
	if (elem)
		if (sectionFromXML(TiXmlHandle(elem), positiveStructures) == 0)
		{
			std::cout << "Difference"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": No positiveStructures" << std::endl;
			return false;
		}
	elem = root.FirstChildElement("Negative").ToElement();
	if (elem)
		if (sectionFromXML(TiXmlHandle(elem), negativeStructures) == 0)
		{
			std::cout << "Difference"
				<< (this->name().empty() ? "" : " \""+this->name()+"\"")
				<< ": No negativeStructures" << std::endl;
			return false;
		}

	return true;
}

template <typename T>
TiXmlElement * const Difference<T>::toXML() const
{
	using boost::lexical_cast;

	TiXmlElement * const root = new TiXmlElement("Difference");

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

	TiXmlElement * const plusElem = new TiXmlElement("Positive");
	for (typename std::vector<Structure<T> *>::const_iterator i = positiveStructures.begin();
	     i != positiveStructures.end(); ++i)
		plusElem->LinkEndChild( (*i)->toXML() );
	root->LinkEndChild( plusElem );

	TiXmlElement * const minusElem = new TiXmlElement("Negative");
	for (typename std::vector<Structure<T> *>::const_iterator i = negativeStructures.begin();
	     i != negativeStructures.end(); ++i)
		minusElem->LinkEndChild( (*i)->toXML() );
	root->LinkEndChild( minusElem );

	return root;
}

template <typename T>
void Difference<T>::print(unsigned indent) const
{
	using boost::lexical_cast;

	Structure<T>::printIndented("<Difference>", indent);
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

	Structure<T>::printIndented("<Plus>\n", indent + 1);
	for (typename std::vector<Structure<T> *>::const_iterator i = positiveStructures.begin();
	     i != positiveStructures.end(); ++i)
		(*i)->print(indent + 2);
	Structure<T>::printIndented("</Plus>\n", indent + 1);

	Structure<T>::printIndented("<Minus>\n", indent + 1);
	for (typename std::vector<Structure<T> *>::const_iterator i = negativeStructures.begin();
	     i != negativeStructures.end(); ++i)
		(*i)->print(indent + 2);
	Structure<T>::printIndented("</Minus>\n", indent + 1);

	Structure<T>::printIndented("</Difference>\n", indent);
}

} // end namespace
