cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
#----------------------------------------------------------------------------
# Setup the project, first we make sure the compiler is the one corresponding 
# to the ups e7 compiler.. 
#
SET(CMAKE_C_COMPILER "$ENV{GCC_FQ_DIR}/bin/gcc")
SET(CMAKE_CXX_COMPILER "$ENV{GCC_FQ_DIR}/bin/g++")

project(g4hp C CXX)


#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{G4LIB} $ENV{ROOTSYS}/etc/cmake)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{G4LIB} $ENV{ROOTSYS}/etc/cmake)
#
find_package(Geant4 REQUIRED)
#
# root..
#
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR}) # should do the trick, but may be not..Needt root v5_34_25a or later
include_directories(${ROOT_LIBRARY_DIR})
include_directories($ENV{ROOTSYS}/include ) #just to make sure..	    
#
#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR} 
		    $ENV{DK2NU_FQ_DIR}/include
		    ${ROOT_INCLUDE_DIR})

file(GLOB sources  ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB hheaders ${PROJECT_SOURCE_DIR}/include/*.hh)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++1y -g -rdynamic " )
#
# At last but not least, CLHEP
#
SET(CLHEPLIBDIR $ENV{CLHEP_LIB_DIR})
#
#MESSAGE(STATUS ROOT_LIBRARIES = ${ROOT_LIBRARIES})
#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
link_directories(${CLHEPLIBDIR}  $ENV{DK2NU_FQ_DIR}/lib ${PROJECT_SOURCE_DIR}/build)
#
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build) # just for libg4hpDict.so
#	
#
add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/g4hpCint.cc COMMAND rootcint -f 
                   g4hpCint.cc -c include/TrackInfo_t.hh include/ProdTuple_t.hh Linkdef.h)
		   
add_library(g4hpDict SHARED ${PROJECT_SOURCE_DIR}/g4hpCint.cc
            ${PROJECT_SOURCE_DIR}/src/TrackInfo_t.cc 
	    ${PROJECT_SOURCE_DIR}/src/ProdTuple_t.cc )
#	    
add_executable(g4hp  g4hp.cc ${sources} ${hheaders} )

target_link_libraries(g4hp g4hpDict ${Geant4_LIBRARIES} libdk2nuTree.so ${root_LIBRARIES} )
				  
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS g4hp  DESTINATION bin)

