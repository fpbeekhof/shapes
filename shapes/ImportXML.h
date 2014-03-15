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

#ifndef SHAPES_IMPORT_XML_H
#define SHAPES_IMPORT_XML_H 1

#include <string>
#include <shapes/tinyxml.h>
#include <shapes/Shape.h>

namespace shapes
{

namespace io {

template <typename T>
bool importXML(const std::string fileName, Shape<T> &shape)
{
	TiXmlDocument doc(fileName.c_str());
	return doc.LoadFile() && shape.fromXml(doc);
}

} // end namespace io

} // end namespace shapes

#endif
