/***************************************************************************
 *   Copyright (C) 2005-2009 by F. P. Beekhof                              *
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

#include <iostream>
#include <cassert>

#include <boost/lexical_cast.hpp>
#include <shapes/tinyxml.h>
#include <shapes/shapes.hpp>

void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 );

void usage(char * const progName)
{
	std::cout << "Usage: " << progName << " <-I|-S|-V|-T> <voxelsize> <XML-file> [output]"
		  << std::endl;
	std::cout << "Usage: " << progName << " <XML-file>" << std::endl;
	exit(1);
}

int main(int argc, char **argv)
{
	using namespace shapes;

	typedef double T;
//	typedef float T;

	if (argc == 2) // XML debugging mode
	{
		TiXmlDocument doc(argv[1]);
		if (!doc.LoadFile())
		{
			std::cout << "Error loading file [" << argv[1] << "]."
				  << std::endl;
			return 1;
		}
		std::cout << "------------ TINYXML DUMP ------------" << std::endl;
		dump_to_stdout(&doc);

		Shape<T> shape;
		if(!shape.fromXml(doc))
		{
			std::cout << "Error converting file [" << argv[1] << "] to a shape." << std::endl;
			return 1;
		}
		std::cout << "------------ SHAPE XML DUMP ------------" << std::endl;
	 	shape.print();
		std::cout << std::endl;
		std::string output(argv[1]);
		if (output.length() > 5)
			output = output.substr(0, output.length() - 4); // strip extention
		output += ".parsed.xml";
		shape.toXml()->SaveFile(output.c_str());

		return 0;
	}

	if ( (argc < 4) || (argc > 5) )
		usage(argv[0]);

	std::string outputMode(argv[1]);
	if ( (outputMode != "-I") && (outputMode != "-S") &&
	     (outputMode != "-V") && (outputMode != "-T") )
		usage(argv[0]);

	const T sampleSize = boost::lexical_cast<T>(argv[2]);
	const std::string input(argv[3]);

	Shape<T> shape;
	if (!io::importXML(input, shape))
	{
		std::cout << "Error loading Shape file [" << input << "]." << std::endl;
		return 1;
	}

	std::string output;
	if (argc == 5)
		output = argv[4];
	else if (input.length() > 5)
		output = input.substr(0, input.length() - 4); // strip extention
	else
		output = input;

	bool ok = false;
	if (outputMode == "-I")
	{
		output += ".itk";
		ok = io::exportITK<T>(output, shape, sampleSize);
	}
	else if (outputMode == "-S")
	{
		if (argc != 5) output += ".stl";
		ok = io::exportSTL<T>(output, shape, sampleSize);
	}
	else if (outputMode == "-V")
	{
		if (argc != 5) output += ".dat";
		ok = io::exportVoxels<T>(output, shape, sampleSize);
	}
	else if (outputMode == "-T")
	{
		if (argc != 5) output += ".tree.xml.zip";//gz";
		ok = io::exportOctree(output, shape, sampleSize);
	}
	else
	{
		assert(false);
		usage(argv[0]);
	}

	if (!ok)
	{
		std::cout << "Error generating distance field from file ["
			  << input << "]." << std::endl;
		return 1;
	}

	return 0;
}

// tutorial demo program
// #include "stdafx.h"

// ----------------------------------------------------------------------
// STDOUT dump and indenting utility functions
// ----------------------------------------------------------------------
const unsigned int NUM_INDENTS_PER_SPACE=2;

const char * getIndent( unsigned int numIndents )
{
	static const char * pINDENT="                                      + ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

// same as getIndent but no "+" at the end
const char * getIndentAlt( unsigned int numIndents )
{
	static const char * pINDENT="                                        ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
{
	if ( !pElement ) return 0;

	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent=getIndent(indent);
	printf("\n");
	while (pAttrib)
	{
		printf( "%s%s: value=[%s]", pIndent, pAttrib->Name(),
			pAttrib->Value());

		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)
			printf( " int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS)
			printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;
}

void dump_to_stdout( TiXmlNode* pParent, unsigned int indent)
{
	if ( !pParent ) return;

	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf( "%s", getIndent(indent));
	int num;

	switch ( t )
	{
	case TiXmlNode::DOCUMENT:
		printf( "Document" );
		break;

	case TiXmlNode::ELEMENT:
		printf( "Element [%s]", pParent->Value() );
		num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
		switch(num)
		{
			case 0:  printf( " (No attributes)"); break;
			case 1:  printf( "%s1 attribute",
					getIndentAlt(indent)); break;
			default: printf( "%s%d attributes",
					getIndentAlt(indent), num); break;
		}
		break;

	case TiXmlNode::COMMENT:
		printf( "Comment: [%s]", pParent->Value());
		break;

	case TiXmlNode::UNKNOWN:
		printf( "Unknown" );
		break;

	case TiXmlNode::TEXT:
		pText = pParent->ToText();
		printf( "Text: [%s]", pText->Value() );
		break;

	case TiXmlNode::DECLARATION:
		printf( "Declaration" );
		break;
	default:
		break;
	}
	printf( "\n" );
	for ( pChild = pParent->FirstChild();
	    pChild; pChild = pChild->NextSibling())
	{
		dump_to_stdout( pChild, indent+1 );
	}
}
