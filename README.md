
* Project Location & Contact

	WWW:    http://github.com/fpbeekhof/shapes
	E-mail: fpbeekhof AT gmail DOT com

* Changelog
	20110116
		- Bugfixes is Sphere and Tube

	20101115-v1.0RC1
		- Requires cvmlcpp 20101115 or later
		- Faster STL rendering
		- New examples
		
	20090720BETA
		- Requires cvmlcpp 20090720  or later
		- Remove Orientation "Axis" in favour of vector/angle
		- Requires boost 1.37 or later
		- GSL error handling
		- BoundingBox added for Shapes

	200904BETA
		- ExportOctree: gzip output, multiple bugfixes
		- Changed meaning of exponent in basic structures
		- Build fix
		- Added guide.html

	20090405BETA
		- Requires cvmlcpp 20090405 or later
		- Restructured directories
		- Update documentation a bit
		- ExportOctree: bugfix; don't manually close file
		- ExportITK: patch from GB; include offset & voxelsize
		- ExportITK: back to previous data-ordering; paraview ok

	20090329BETA
		- Completely revised beta release: all template code
		- Use GSL to speed up calculations
		- Many bug-fixes
		- Much reduced code-base, uses CVMLCPP
		- Implemented mathematic model by Guntram Berti (NEC)
		- Incorporated patches from Guntram Berti (NEC)

	20060831
		- New File Format
		- Smooth decay to zero contributed by Guntram Berti
		- General ellipsoid support suggested by Guntram Berti

	20060828
		- New file format
		- Removed GUI
		- Removed Configuration class, integrated tinyXML parser
		- Support for Union
		- Restructured code: Tube / Segment / TubeGenerator

	20060815
		- Included ExportITK code contributed by Guntram Berti
		- Added OpenMP support
		- Added Command-Line converter
		- Added getCenterDerivative() to Tube
		- Restructured the code: Shape, Export*, FieldGenerator

	20060811
		- Initial release

* TODO / Known Bugs

- Do something about bounding box of the geometry, either user specified or 
correctly estimated.
- ITK might be broken
