/*
 * Contribution by Guntram Berti,
 * Modifications by Fokko Beekhof
 */

/*! \brief Export distance map into MetaImage format

     See http://caddlab.rad.unc.edu/software/MetaIO/MetaIO-Introduction.htm

 */

// This URI is dead FPB/2009

#ifndef SHAPES_EXPORT_ITK_H
#define SHAPES_EXPORT_ITK_H 1

#include <fstream>
#include <string>
#include <cvmlcpp/base/Matrix>

#include <shapes/ExportField.h>

namespace shapes
{

namespace io {

template <typename T>
bool exportITK(const std::string fileName, const Shape<T> &shape,
		const T sampleSize = 1.)
{
	// Generate the field
	cvmlcpp::Matrix<T, 3> field;
	if (!convertToField(shape, sampleSize, field))
		return false;

	// Get constants, we need the offsets delta*
	std::size_t dimX, dimY, dimZ;
	T deltaX, deltaY, deltaZ;
	calcShapeConsts(shape, sampleSize, dimX, dimY, dimZ,
			deltaX, deltaY, deltaZ);

	// we must know the voxel size! Seems to be always 1 ...
	const float vx = sampleSize;
	const float vy = sampleSize;
	const float vz = sampleSize;

	// where do we get the offset? Must keep track of deltax + padding
	const float ox = deltaX+0.5f*vx;
	const float oy = deltaY+0.5f*vy;
	const float oz = deltaZ+0.5f*vz;

	// note: Format consist of two files, an ASCII header
	// and a binary data file.

	// write header
	std::string header_name = fileName + ".mhd";
	std::ofstream out_h(header_name.c_str());
	out_h << "ObjectType = Image\n"
		<< "NDims = 3\n"
		<< "BinaryData = True\n"
		<< "BinaryDataByteOrderMSB = False\n"
		<< "ElementSize = " << vx << " " << vy << " " << vz <<  "\n"
		<< "Offset      = " << ox << " " << oy << " " << oz <<  "\n"
		<< "TransformMatrix = 1 0 0  0 1 0  0 0 1 \n"
		<< "DimSize = "
		<< field.extent(X) << " "
		<< field.extent(Y) << " "
		<< field.extent(Z) << "\n"
		<< "ElementType = MET_FLOAT\n"
		<< "ElementDataFile = " << fileName << "\n"
		<< std::flush;

	out_h.close();

	// write binary raw data file
	std::ofstream output(fileName.c_str(),  std::ios::out    |
						std::ios::binary |
						std::ios::trunc);

	for(std::size_t z = 0; z < field.extent(Z); ++z)
	for(std::size_t y = 0; y < field.extent(Y); ++y)
	for(std::size_t x = 0; x < field.extent(X); ++x)
	{
		// note: data layout expects x changing fastest.
		// the extremely nonlinear values cause visualisation problems
		// attempt to transform mapping 1 -> 0, inside < 0, outside > 0
		// float eps = 0.1;
		// val = 1.0/(eps+val) - 1.0/(1.0 + eps);
		float val = field[x][y][z];
		output.write((char *)&val, sizeof(float));
	}
	output.close();

	return true;
}

} // end namespace io

} // end namespace shapes

#endif
