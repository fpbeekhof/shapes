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
#include <iostream>

#include <boost/lexical_cast.hpp>

#include <shapes/Shape.h>
#include <shapes/Sphere.h>
#include <shapes/Tube.h>
#include <shapes/Union.h>
#include <shapes/Intersection.h>
#include <shapes/Difference.h>

namespace shapes
{

template <typename T>
TiXmlDocument * const Shape<T>::toXml() const
{
	TiXmlDocument	*doc = new TiXmlDocument();
 	TiXmlDeclaration*decl = new TiXmlDeclaration( "1.0", "", "" );

	doc->LinkEndChild( decl );

	// Root tag
	TiXmlElement * root = new TiXmlElement("Shape");
	doc->LinkEndChild( root );

	if (structure_ != NULL)
		root->LinkEndChild(structure_->toXML());


	if (boxSet)
	{
		using boost::lexical_cast;
		TiXmlElement * const bbElem = new TiXmlElement("BoundingBox");

		TiXmlElement * const minCornerElem = new TiXmlElement("MinCorner");
		std::string buf = lexical_cast<std::string>(minCorner.x()) + " " +
				  lexical_cast<std::string>(minCorner.y()) + " " +
				  lexical_cast<std::string>(minCorner.z());
		minCornerElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		bbElem->LinkEndChild(minCornerElem);

		TiXmlElement * const maxCornerElem = new TiXmlElement("MaxCorner");
		buf =	lexical_cast<std::string>(maxCorner.x()) + " " +
			lexical_cast<std::string>(maxCorner.y()) + " " +
			lexical_cast<std::string>(maxCorner.z());
		maxCornerElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		bbElem->LinkEndChild(maxCornerElem);
		root->LinkEndChild(bbElem);
	}

	return doc;
}

template <typename T>
bool Shape<T>::fromXml(TiXmlDocument &doc)
{
	TiXmlHandle docHandle(&doc);

	TiXmlHandle root = docHandle.FirstChildElement("Shape");

	// Load Spheres
	unsigned spheres = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Sphere").ToElement();
	     elem; elem = elem->NextSiblingElement("Sphere") )
	{
		++spheres;
		std::tr1::shared_ptr<Structure<T> > sphere(new Sphere<T>());
		TiXmlHandle	handle(elem);
		if (!sphere->fromXML(handle))
		{
			std::cout << "Shape: parse failure Sphere "
				<< spheres << "." << std::endl;
			return false;
		}

		structure_ = sphere;
	}

	// Load Tubes
	unsigned tubes = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Tube").ToElement();
	     elem; elem = elem->NextSiblingElement("Tube") )
	{
		++tubes;
		std::tr1::shared_ptr<Structure<T> > tube(new Tube<T>());
		TiXmlHandle	handle(elem);
		if (!tube->fromXML(handle))
		{
			std::cout << "Shape: parse failure Tube "
				<< tubes << "." << std::endl;
			return false;
		}

		structure_ = tube;
	}

	// Load Unions
	unsigned unions = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Union").ToElement();
	     elem; elem = elem->NextSiblingElement("Union") )
	{
		++unions;
		std::tr1::shared_ptr<Structure<T> > _union(new Union<T>());
		TiXmlHandle	handle(elem);
		if (!_union->fromXML(handle))
		{
			std::cout << "Shape: parse failure Union "
				<< unions << "." << std::endl;
			return false;
		}

		structure_ = _union;
	}

	// Load Intersections
	unsigned intersections = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Intersection").ToElement();
	     elem; elem = elem->NextSiblingElement("Intersection") )
	{
		++intersections;
		std::tr1::shared_ptr<Structure<T> > _intersection(new Intersection<T>());
		TiXmlHandle	handle(elem);
		if (!_intersection->fromXML(handle))
		{
			std::cout << "Shape: parse failure Intersection "
				<< intersections << "." << std::endl;
			return false;
		}

		structure_ = _intersection;
	}

	// Load Differences
	unsigned differences = 0;
	for (TiXmlElement * elem = root.FirstChildElement("Difference").ToElement();
	     elem; elem = elem->NextSiblingElement("Difference") )
	{
		++differences;
		std::tr1::shared_ptr<Structure<T> > _difference(new Difference<T>());
		TiXmlHandle	handle(elem);
		if (!_difference->fromXML(handle))
		{
			std::cout << "Shape: parse failure Difference "
				<< differences << "." << std::endl;
			return false;
		}

		structure_ = _difference;
	}

	if (spheres+tubes+unions+intersections+differences != 1)
	{
		std::cout << "Shape: there must be one single main structure." << std::endl;
		return false;
	}

	TiXmlElement * boxElem = root.FirstChildElement("BoundingBox").ToElement();
	if (boxElem)
	{
		if (!boxElem->FirstChildElement("MinCorner"))
		{
			std::cout << "Shape: no MinCorner in BoundingBox." << std::endl;
			return false;
		}
		TiXmlElement * minBoxElem = boxElem->FirstChildElement("MinCorner")->ToElement();

		std::stringstream smin(minBoxElem->GetText());
		smin >> minCorner.x();
		smin >> minCorner.y();
		smin >> minCorner.z();
		minBoxElem = minBoxElem->NextSiblingElement("MinCorner");
		if (minBoxElem)
		{
			std::cout << "Shape: more than one MinCorner found in BoundingBox." << std::endl;
			return false;
		}

		if (!boxElem->FirstChildElement("MaxCorner"))
		{
			std::cout << "Shape: no MaxCorner in BoundingBox." << std::endl;
			return false;
		}
		TiXmlElement * maxBoxElem = boxElem->FirstChildElement("MaxCorner")->ToElement();

		std::stringstream smax(maxBoxElem->GetText());
		smax >> maxCorner.x();
		smax >> maxCorner.y();
		smax >> maxCorner.z();

		maxBoxElem = maxBoxElem->NextSiblingElement("MaxCorner");
		if (maxBoxElem)
		{
			std::cout << "Shape: more than one MinCorner found in BoundingBox." << std::endl;
			return false;
		}

		boxSet = true;
		boxElem = boxElem->NextSiblingElement("BoundingBox");
		if (boxElem)
		{
			std::cout << "Shape: more than one BoundingBox found." << std::endl;
			return false;
		}
	}

	return true;
}

template <typename T>
void Shape<T>::add(Structure<T> * const structure)
{
	structure_ = std::tr1::shared_ptr<Structure<T> >(structure);
	boxCached = false;
}

template <typename T>
void Shape<T>::clear()
{
	structure_ = std::tr1::shared_ptr<Structure<T> >();
}

template <typename T>
void Shape<T>::getBoundingBox(FPPoint &_minCorner, FPPoint &_maxCorner) const
{
	if (!boxSet && !boxCached)
	{
		structure_->getBoundingBox(minCorner, maxCorner);
		FPPoint center = (minCorner + maxCorner) / 2.;
		minCorner = center + 1.1*(minCorner - center);
		maxCorner = center + 1.1*(maxCorner - center);

		boxCached = true;
	}

	_minCorner = minCorner;
	_maxCorner = maxCorner;
}

template <typename T>
void Shape<T>::print() const
{
	std::cout << "<?xml version=\"1.0\" ?>" << std::endl << std::endl;
	std::cout << "<Shape>" << std::endl;

	structure_->print(1);

	if (boxSet)
	{
		std::cout << "\t<BoundingBox>" << std::endl;
		std::cout << "\t\t<MinCorner>" << minCorner.x() << " "
			<< minCorner.y() << " " << minCorner.z() << "</MinCorner>" << std::endl;
		std::cout << "\t\t<MaxCorner>" << maxCorner.x() << " "
			<< maxCorner.y() << " " << maxCorner.z() << "</MaxCorner>" << std::endl;
		std::cout << "\t</BoundingBox>" << std::endl;
	}

	std::cout << "</Shape>" << std::endl;
}

} // end namespace
