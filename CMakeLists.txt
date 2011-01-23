cmake_minimum_required(VERSION 2.8)

project(fgrun)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

file(READ version versionFile)
string(STRIP ${versionFile} FGRUN_VERSION) 

set(MSVC_3RDPARTY_DIR NOT_FOUND CACHE PATH "Location where the third-party dependencies are extracted")

if (MSVC_3RDPARTY_DIR)
  message(STATUS "3rdparty files located in ${MSVC_3RDPARTY_DIR}")
  set (CMAKE_LIBRARY_PATH ${MSVC_3RDPARTY_DIR}/3rdParty/lib ${MSVC_3RDPARTY_DIR}/install/msvc90/OpenScenegraph/lib ${MSVC_3RDPARTY_DIR}/install/msvc90/SimGear/lib )
  set (CMAKE_INCLUDE_PATH ${MSVC_3RDPARTY_DIR}/3rdParty/include ${MSVC_3RDPARTY_DIR}/install/msvc90/OpenScenegraph/include ${MSVC_3RDPARTY_DIR}/install/msvc90/SimGear/include)
  set (BOOST_ROOT ${MSVC_3RDPARTY_DIR}/boost_1_44_0)
endif (MSVC_3RDPARTY_DIR)

if(WIN32)

    if(MSVC)
        set(MSVC_FLAGS "-DNOMINMAX -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D__CRT_NONSTDC_NO_WARNINGS")
    endif(MSVC)

    set(NOMINMAX 1)
    set( WINSOCK_LIBRARY "ws2_32.lib" )
endif(WIN32)    

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT -DVERSION=\\\"${FGRUN_VERSION}\\\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT -DVERSION=\\\"${FGRUN_VERSION}\\\"")

find_package(Boost REQUIRED)
find_package(ZLIB REQUIRED)
find_package(FLTK REQUIRED)
find_package(OpenSceneGraph 2.8.1 REQUIRED osgText osgDB osgParticle osgUtil osgViewer osgGA)
find_package(SimGear 2.2.0 REQUIRED)

include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIR}
	${SIMGEAR_INCLUDE_DIR}
    ${FLTK_INCLUDE_DIR})

add_subdirectory( src )
