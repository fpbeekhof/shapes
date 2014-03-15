#####################################
## Users: Do not change this file! ##
#####################################

# Receive from Makefile: Location of CVMLCPP Installation
CVMLCPP_PREFIX = ARGUMENTS.get( 'CVMLCPP_PREFIX',   '/usr/local' )

# Have a building environment contructed by CVMLCPP build system
env = SConscript(CVMLCPP_PREFIX+'/share/cvmlcpp/build/SConscript')

# Receive from Makefile: Directory to install
TARGET = ARGUMENTS.get('TARGET', '/usr/local')

# Receive from Makefile: Which backend should we use ?
backend = ARGUMENTS.get('BACKEND', '')

if env.has_key('LIBS'):
	env['LIBS']+=['cvmlcpp', 'libz', 'boost_iostreams']
else:
	env.Append(LIBS=['cvmlcpp', 'libz', 'boost_iostreams'])

if backend == 'gsl':
	env['CXXFLAGS'] += " -DUSE_GSL"
	env['LINKFLAGS'] += " -DUSE_GSL"
	if env.has_key('LIBS'):
		env['LIBS'] += ['gsl', 'gslcblas']
	else:
		env['LIBS']=['gsl', 'gslcblas']

if CVMLCPP_PREFIX != '/usr/' and CVMLCPP_PREFIX != '/usr/local/' and \
   CVMLCPP_PREFIX != '/usr' and CVMLCPP_PREFIX != '/usr/local' and \
   CVMLCPP_PREFIX != '':
	env['CXXFLAGS']  += " -I"+CVMLCPP_PREFIX+'/include'
	env['LINKFLAGS'] += " -L"+CVMLCPP_PREFIX+'/lib'

# Install Directories
TARGETBIN = TARGET + '/bin'
TARGETLIB = TARGET + '/lib'
TARGETINC = TARGET + '/include/shapes'
TARGETDOC = TARGET + '/share/doc/shapes'

SRC    = Glob('shapes/*.h') + Glob('shapes/*.hh')
XMLSRC = ['tinyxml/tinystr.cpp', 'tinyxml/tinyxml.cpp', 'tinyxml/tinyxmlerror.cpp',
	  'tinyxml/tinyxmlparser.cpp', ]

staticLib = env.StaticLibrary( 'lib/shapes', XMLSRC, LIBS=['cvmlcpp', 'libz', 'boost_iostreams-mt'])
sharedLib = env.SharedLibrary( 'lib/shapes', XMLSRC, LIBS=['cvmlcpp', 'libz', 'boost_iostreams-mt'])
prog = env.Program('bin/shapes', ['src/main.cpp'] + staticLib)

env.Install( TARGETDOC, 'doc' )
env.Install( TARGETINC, SRC )
env.Install( TARGETLIB, [staticLib, sharedLib] )
env.Install( TARGETBIN, prog )
env.Alias('install', [TARGETINC, TARGETBIN, TARGETLIB])
