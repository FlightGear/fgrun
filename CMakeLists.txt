cmake_minimum_required(VERSION 2.8)

include (CheckIncludeFile)

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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT")

find_package(Boost REQUIRED)
find_package(ZLIB REQUIRED)
find_package(FLTK REQUIRED)
find_package(OpenSceneGraph 2.8.1 REQUIRED osgText osgDB osgParticle osgUtil osgViewer osgGA)
find_package(SimGear 2.2.0 REQUIRED)
find_package(Gettext)

if (GETTEXT_FOUND)

    FIND_LIBRARY(INTL_LIBRARY
      NAMES intl
      HINTS
      PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
      PATHS
      ~/Library/Frameworks
      /Library/Frameworks
      /usr/local
      /usr
      /opt
    )
    
    if (INTL_LIBRARY)
        set( ENABLE_NLS 1 )
    endif (INTL_LIBRARY)
    
endif (GETTEXT_FOUND)

check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(sys/wait.h HAVE_SYS_WAIT_H)
check_include_file(termios.h HAVE_TERMIOS_H)

add_definitions(-DHAVE_CONFIG_H)

include_directories(${PROJECT_BINARY_DIR}/src)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config_cmake.h.in"
  "${PROJECT_BINARY_DIR}/src/config.h"
  )


include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
	${ZLIB_INCLUDE_DIR}
	${SIMGEAR_INCLUDE_DIR}
    ${FLTK_INCLUDE_DIR})

add_subdirectory( src )
add_subdirectory( po )
