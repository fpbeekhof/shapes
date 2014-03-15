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

#include <string>
#include <fstream>
#include <iostream>
#include <cassert>

#include <boost/iostreams/filtering_stream.hpp>
//#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <shapes/shapes.hpp>

cvmlcpp::DTree<short int, 3> readVoxTree(std::string fileName)
{
	cvmlcpp::DTree<short int, 3> voxtree;

	std::ifstream f(fileName.c_str());
	boost::iostreams::filtering_istream in;
//	in.push(boost::iostreams::zlib_decompressor());
	in.push(boost::iostreams::gzip_decompressor());
	in.push(f);

	in >> voxtree;

	return voxtree;
}

int main()
{
	shapes::Shape<double> shape;
	assert(shapes::io::importXML("../doc/Example.xml", shape));
	assert(shapes::io::exportOctree("/tmp/testvoxtree.xml.gz", shape));

	cvmlcpp::DTree<short int, 3> readTree = readVoxTree("/tmp/testvoxtree.xml.gz");

	return 0;
}

