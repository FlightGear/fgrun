# Locate SimGear
# This module defines

# SIMGEAR_CORE_LIBRARIES, a list of the core static libraries
# SIMGEAR_LIBRARIES, a list of all the static libraries (core + scene)
# SIMGEAR_FOUND, if false, do not try to link to SimGear
# SIMGEAR_INCLUDE_DIR, where to find the headers
#
# $SIMGEAR_DIR is an environment variable that would
# correspond to the ./configure --prefix=$SIMGEAR_DIR
# used in building SimGear.
#
# Created by James Turner. This was influenced by the FindOpenAL.cmake module.

#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

include(SelectLibraryConfigurations)

macro(find_sg_library libName varName libs)
    set(libVarName "${varName}_LIBRARY")
    
    FIND_LIBRARY(${libVarName}_DEBUG
      NAMES ${libName}${CMAKE_DEBUG_POSTFIX}
      HINTS $ENV{SIMGEAR_DIR}
      PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR} libs64 libs libs/Win32 libs/Win64
      PATHS
      /usr/local
      /usr
      /opt
    )
    FIND_LIBRARY(${libVarName}_RELEASE
      NAMES ${libName}${CMAKE_RELEASE_POSTFIX}
      HINTS $ENV{SIMGEAR_DIR}
      PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR} libs64 libs libs/Win32 libs/Win64
      PATHS
      /usr/local
      /usr
      /opt
    )
    
   # message(STATUS "before: Simgear ${${libVarName}_RELEASE} ")
  #  message(STATUS "before: Simgear ${${libVarName}_DEBUG} ")
    
    select_library_configurations( ${varName} )

  #  message(STATUS "after:Simgear ${${libVarName}_RELEASE} ")
  #  message(STATUS "after:Simgear ${${libVarName}_DEBUG} ")

    set(componentLibRelease ${${libVarName}_RELEASE})
  #  message(STATUS "Simgear ${libVarName}_RELEASE ${componentLibRelease}")
    set(componentLibDebug ${${libVarName}_DEBUG})
   # message(STATUS "Simgear ${libVarName}_DEBUG ${componentLibDebug}")
    
    if (NOT ${libVarName}_DEBUG)
        if (NOT ${libVarName}_RELEASE)
            #message(STATUS "found ${componentLib}")
            list(APPEND ${libs} ${componentLibRelease})
        endif()
    else()
        list(APPEND ${libs} optimized ${componentLibRelease} debug ${componentLibDebug})
    endif()
endmacro()

macro(find_sg_component comp libs)
    set(compLib "sg${comp}")
    string(TOUPPER "SIMGEAR_${comp}" libVar)
    
    find_sg_library(${compLib} ${libVar} ${libs})
endmacro()

FIND_PATH(SIMGEAR_INCLUDE_DIR simgear/math/SGMath.hxx
  HINTS $ENV{SIMGEAR_DIR}
  PATH_SUFFIXES include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /opt
)

# message(STATUS ${SIMGEAR_INCLUDE_DIR})

# dependent packages
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

if(SIMGEAR_SHARED)
    message(STATUS "looking for shared Simgear libraries")

    find_sg_library(SimGearCore SIMGEAR_CORE SIMGEAR_CORE_LIBRARIES)
    find_sg_library(SimGearScene SIMGEAR_SCENE SIMGEAR_LIBRARIES)

 
    list(APPEND SIMGEAR_LIBRARIES ${SIMGEAR_CORE_LIBRARIES})
    set(SIMGEAR_CORE_LIBRARY_DEPENDENCIES "")
    set(SIMGEAR_SCENE_LIBRARY_DEPENDENCIES "")
    
   # message(STATUS "core lib ${SIMGEAR_CORE_LIBRARIES}")
  #  message(STATUS "all libs ${SIMGEAR_LIBRARIES}")
else(SIMGEAR_SHARED)

    set(SIMGEAR_LIBRARIES "") # clear value
    set(SIMGEAR_CORE_LIBRARIES "") # clear value
    message(STATUS "looking for static Simgear libraries")
    
  # note the order here affects the order Simgear libraries are
  # linked in, and hence ability to link when using a traditional
  # linker such as GNU ld on Linux
    set(comps
        environment
        nasal
        bucket
        route
        timing
        io
        serial
        math
        props
        structure
        xml
        misc
        threads
        debug
        magvar
    )

    set(scene_comps
        tsync
        ephem
        sky
        material
        tgdb
        model
        screen
        bvh
        util
        sound)
            
    foreach(component ${comps})
        find_sg_component(${component} SIMGEAR_CORE_LIBRARIES)
    endforeach()

    foreach(component ${scene_comps})
        find_sg_component(${component} SIMGEAR_LIBRARIES)
    endforeach()

    
    # again link order matters - scene libraries depend on core ones
    list(APPEND SIMGEAR_LIBRARIES ${SIMGEAR_CORE_LIBRARIES})

    #message(STATUS "all libs ${SIMGEAR_LIBRARIES}")
    
    set(SIMGEAR_CORE_LIBRARY_DEPENDENCIES
        ${CMAKE_THREAD_LIBS_INIT}
        ${ZLIB_LIBRARY})

    set(SIMGEAR_SCENE_LIBRARY_DEPENDENCIES 
        ${ALUT_LIBRARY} 
    	${OPENAL_LIBRARY}
    	${LIBSVN_LIBRARIES})

    if(WIN32)
        list(APPEND SIMGEAR_CORE_LIBRARY_DEPENDENCIES ws2_32.lib)
    endif(WIN32)

    if(NOT MSVC)
        # basic timing routines on non windows systems, may be also cygwin?!
        check_function_exists(clock_gettime clock_gettime_in_libc)
        if(NOT clock_gettime_in_libc)
            check_library_exists(rt clock_gettime "" have_rt)
            if(have_rt)
                list(APPEND SIMGEAR_CORE_LIBRARY_DEPENDENCIES rt)
            endif(have_rt)
        endif(NOT clock_gettime_in_libc)
    endif(NOT MSVC)
endif(SIMGEAR_SHARED)

# now we've found SimGear, check its version

include(CheckCXXSourceRuns)

message(STATUS "looking for version: ${SimGear_FIND_VERSION}")

SET(CMAKE_REQUIRED_INCLUDES ${SIMGEAR_INCLUDE_DIR})

check_cxx_source_runs(
    "#include <cstdio>
    #include \"simgear/version.h\"

    #define xstr(s) str(s)
    #define str(s) #s

    #define MIN_MAJOR ${SimGear_FIND_VERSION_MAJOR}
    #define MIN_MINOR ${SimGear_FIND_VERSION_MINOR}
    #define MIN_MICRO ${SimGear_FIND_VERSION_PATCH}

    int main() {
        int major, minor, micro;

        /* printf(%d.%d.%d or greater, , MIN_MAJOR, MIN_MINOR, MIN_MICRO); */
        printf(\"found %s ... \", xstr(SIMGEAR_VERSION));

        sscanf( xstr(SIMGEAR_VERSION), \"%d.%d.%d\", &major, &minor, &micro );

        if ( (major < MIN_MAJOR) ||
             (major == MIN_MAJOR && minor < MIN_MINOR) ||
             (major == MIN_MAJOR && minor == MIN_MINOR && micro < MIN_MICRO) ) {
         return -1;
        }

        return 0;
    }
    "
    SIMGEAR_VERSION_OK)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SimGear DEFAULT_MSG
     SIMGEAR_LIBRARIES SIMGEAR_INCLUDE_DIR SIMGEAR_VERSION_OK)

