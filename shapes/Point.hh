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
#include <sstream>

#include <boost/lexical_cast.hpp>

namespace shapes
{

template <typename T>
Point<T>::Point(const FPPoint &_center, const FPVector &_weight,
//		const FPVector _orientation[3],
		const T _R,
		const FPVector _rotVector, const T _angle,
		const T _exponent) :
	center(_center), weight(_weight), R(_R), rotVector(_rotVector), angle(_angle), exponent(_exponent)
{
	assert(cvmlcpp::modulus(weight) > 0.);
	assert(cvmlcpp::modulus(rotVector) > 0.);

	weight *= std::sqrt(T(3)) / cvmlcpp::modulus(weight);
	rotVector /= cvmlcpp::modulus(rotVector);
/*
	for (unsigned i = 0; i < 3; ++i)
	{
		orientation[i] = _orientation[i];
		orientation[i] /= cvmlcpp::modulus(orientation[i]);
	}
	this->recomputeOrientation();
*/
}

template <typename T>
bool Point<T>::set(const FPPoint &_center, const FPVector &_weight,
//		const FPVector _orientation[3],
		const T _R,
		const FPVector _rotVector, const T _angle,
		const T _exponent)
{
	const T norm_rotVector = cvmlcpp::modulus(rotVector);
	if (! (norm_rotVector > 0) );
		return false;
	const T norm_weight = cvmlcpp::modulus(_weight);
	if (! (norm_weight > 0) );
		return false;
	rotVector /= norm_rotVector;
	weight    = _weight * std::sqrt(T(3)) / norm_weight;

	center	  = _center;
	R	  = _R;
	exponent  = _exponent;
	angle	  = _angle;
	rotVector = _rotVector;
/*	for (unsigned i = 0; i < 3; ++i)
	{
		orientation[i] = _orientation[i];
		orientation[i] /= cvmlcpp::modulus(orientation[i]);
	}
*/
	return true;
}

template <typename T>
bool Point<T>::fromXml(TiXmlHandle &root)
{
	// Load Center
	TiXmlElement * elem = root.FirstChildElement( "Center" ).ToElement();
	if (!elem)
	{
		std::cout << "Point: parse error: Center must occur once." << std::endl;
		return false;
	}

	std::stringstream scenter(elem->GetText());
	scenter >> center.x();
	scenter >> center.y();
	scenter >> center.z();

	// Load Weight
	elem = root.FirstChildElement( "Weight" ).ToElement();
	if (elem)
	{
		std::stringstream sweight(elem->GetText());
		sweight >> weight.x();
		sweight >> weight.y();
		sweight >> weight.z();
		for (int i = 0; i < 3; ++i) // All weights must be greater than 0
			if (! (weight[i] > 0.))
			{
				const char dim [] = "XYZ";
				std::cout << "Point: parse failure, weight dimension " << dim[i]
					  << " not greater than zero." << std::endl;
				return false;
			}

		// Normalize
		weight *= std::sqrt(T(3)) / cvmlcpp::modulus(weight);
	}

	// Load Orientation
	angle = 0;
	rotVector[X] = 1; rotVector[Y] = rotVector[Z] = 0;
	elem = root.FirstChildElement( "Orientation" ).ToElement();
	if (elem)
	{
/*
		TiXmlHandle axisHandle(elem);
		TiXmlElement * axisElem =
				axisHandle.FirstChildElement("Axis").ToElement();
		for (unsigned i = 0; i < 3; ++i)
		{
			if (!axisElem)
			{
				std::cout << "Point: parse Orientation failure."
					<< std::endl;
				return false;
			}

			std::stringstream sorientation(axisElem->GetText());
			sorientation >> orientation[i].x();
			sorientation >> orientation[i].y();
			sorientation >> orientation[i].z();

			orientation[i] /= cvmlcpp::modulus(orientation[i]);

			axisElem = axisElem->NextSiblingElement("Axis");
		}
*/
		TiXmlHandle orientationHandle(elem);
		TiXmlElement * vecElem =
				orientationHandle.FirstChildElement("RotationVector").ToElement();
		if (!vecElem)
		{
			std::cout << "Point: Orientation parse failure, RotationVector not found."
				<< std::endl;
			return false;
		}
		std::stringstream sorientation(vecElem->GetText());
		sorientation >> rotVector.x();
		sorientation >> rotVector.y();
		sorientation >> rotVector.z();

		// Convert degrees to radians
		TiXmlElement * angleElem =
				orientationHandle.FirstChildElement("Angle").ToElement();
		angle = cvmlcpp::Constants<T>::pi() * boost::lexical_cast<T>(angleElem->GetText()) / T(180);
	}
//	this->recomputeOrientation();

	// Load Radius
	elem = root.FirstChildElement( "Radius" ).ToElement();
	if (elem)
		R = boost::lexical_cast<T>(elem->GetText());
	if (!(R > T(0)))
	{
		std::cout << "Point: Radius parse failure, Radius not greater than zero." << std::endl;
		return false;
	}

	// Load Exponent
	elem = root.FirstChildElement( "Exponent" ).ToElement();
	if (elem)
		exponent = boost::lexical_cast<T>(elem->GetText());
	if (!(exponent > T(0)))
	{
		std::cout << "Point: Exponent parse failure, Exponent not greater than zero." << std::endl;
		return false;
	}

	return true;
}

template <typename T>
TiXmlElement * const Point<T>::toXML(const std::string rootTag) const
{
	using boost::lexical_cast;

	TiXmlElement * const root = new TiXmlElement(rootTag.c_str());

	std::string buf;

	TiXmlElement * const centerElem = new TiXmlElement("Center");
	buf =   lexical_cast<std::string>(this->center.x())+" " +
		lexical_cast<std::string>(this->center.y())+" " +
		lexical_cast<std::string>(this->center.z());
	centerElem->LinkEndChild( new TiXmlText(buf.c_str()) );
	root->LinkEndChild(centerElem);

	if (weight.x() != 1 || weight.y() != 1 || weight.z() != 1)
	{
		buf =   lexical_cast<std::string>(this->weight.x())+" " +
			lexical_cast<std::string>(this->weight.y())+" " +
			lexical_cast<std::string>(this->weight.z());
		TiXmlElement * const weightElem = new TiXmlElement("Weight");
		weightElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		root->LinkEndChild(weightElem);
	}

	// Orientation
	if (angle != 0)
	{
		TiXmlElement * const orientationElem = new TiXmlElement("Orientation");
	/*
		for (unsigned i = 0; i < 3; ++i)
		{
			buf =   lexical_cast<std::string>(this->orientation[i].x())+" " +
				lexical_cast<std::string>(this->orientation[i].y())+" " +
				lexical_cast<std::string>(this->orientation[i].z());
			TiXmlElement * const axisElem = new TiXmlElement("Axis");
			axisElem->LinkEndChild( new TiXmlText(buf.c_str()) );
			orientationElem->LinkEndChild( axisElem );
		}
	*/
		buf =   lexical_cast<std::string>(this->rotVector.x())+" " +
			lexical_cast<std::string>(this->rotVector.y())+" " +
			lexical_cast<std::string>(this->rotVector.z());
		TiXmlElement * const vecElem = new TiXmlElement("RotationVector");
		vecElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		orientationElem->LinkEndChild( vecElem );

		TiXmlElement * const angleElem = new TiXmlElement("Angle");
		angleElem->LinkEndChild( new TiXmlText( // Convert radians to degrees!!
			lexical_cast<std::string>(T(180) * angle / cvmlcpp::Constants<T>::pi()).c_str() ) );
		orientationElem->LinkEndChild( angleElem );

		root->LinkEndChild(orientationElem);
	}

	// Radius
	buf = lexical_cast<std::string>(this->R);
	TiXmlElement * const radiusElem = new TiXmlElement("Radius");
	radiusElem->LinkEndChild( new TiXmlText(buf.c_str()) );
	root->LinkEndChild(radiusElem);

	// Exponent
	if (exponent != 1)
	{
		buf = lexical_cast<std::string>(this->exponent);
		TiXmlElement * const exponentElem = new TiXmlElement("Exponent");
		exponentElem->LinkEndChild( new TiXmlText(buf.c_str()) );
		root->LinkEndChild(exponentElem);
	}

	return root;
}

template <typename T>
void Point<T>::printIndented(const unsigned indent) const
{
/*	for (unsigned i = 0; i < indent; ++i)
		std::cout << "\t";
	std::cout << "<" << rootTag << ">" << std::endl;
*/
	// Center
	for (unsigned i = 0; i <= indent; ++i)
		std::cout << "\t";
	std::cout << "<Center>" << center.x() << " " << center.y() << " "
		  << center.z() << "</Center>" << std::endl;

	// Weight
	if (weight.x() != 1 || weight.y() != 1 || weight.z() != 1)
	{
		for (unsigned i = 0; i <= indent; ++i)
			std::cout << "\t";
		std::cout << "<Weight>" << weight.x() << " " << weight.y() << " "
			  << weight.z() << "</Weight>" << std::endl;
	}

	// Orientation
	if (angle != 0)
	{
		for (unsigned i = 0; i <= indent; ++i)
			std::cout << "\t";
		std::cout << "<Orientation>" << std::endl;
	/*
		for (unsigned d = 0; d < 3; ++d)
		{
			for (unsigned i = 0; i <= indent + 1; ++i)
				std::cout << "\t";

			std::cout << "<Axis>" << orientation[d].x() << " "
				<< orientation[d].y() << " "
				<< orientation[d].z() << "</Axis>" << std::endl;
		}
	*/
		for (unsigned i = 0; i <= indent + 1; ++i)
			std::cout << "\t";
		std::cout << "<RotationVector>" << rotVector.x() << " "
			<< rotVector.y() << " "
			<< rotVector.z() << "</RotationVector>" << std::endl;
		for (unsigned i = 0; i <= indent + 1; ++i)
			std::cout << "\t";
		std::cout << "<Angle>" << (T(180) * angle / cvmlcpp::Constants<T>::pi()) << "</Angle>" << std::endl;

		for (unsigned i = 0; i <= indent; ++i)
			std::cout << "\t";
		std::cout << "</Orientation>" << std::endl;
	}

	// Radius
	for (unsigned i = 0; i <= indent; ++i)
		std::cout << "\t";
	std::cout << "<Radius>" << R << "</Radius>" << std::endl;

	// Exponent
	if (exponent != 1)
	{
		for (unsigned i = 0; i <= indent; ++i)
			std::cout << "\t";
		std::cout << "<Exponent>" << exponent << "</Exponent>" << std::endl;
	}
/*
	for (unsigned i = 0; i < indent; ++i)
		std::cout << "\t";
	std::cout << "</" << rootTag << ">" << std::endl;
*/
}

} // end namespace

