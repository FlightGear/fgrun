cmake_minimum_required(VERSION 2.6.4)

include (CheckIncludeFile)
include (CheckFunctionExists)

project(fgrun)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

if(${CMAKE_VERSION} VERSION_GREATER 2.8.4)
    # use official include provided by latest CMake
    include(GNUInstallDirs)
else(${CMAKE_VERSION} VERSION_GREATER 2.8.4)
    # backward compatibility: use our own module for older cmake versions
    include(OldGNUInstallDirs)
endif(${CMAKE_VERSION} VERSION_GREATER 2.8.4)

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")

file(READ version versionFile)
string(STRIP ${versionFile} FGRUN_VERSION)

#packaging
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
SET(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FlightGear Launcher")
SET(CPACK_PACKAGE_VENDOR "The FlightGear project")
SET(CPACK_GENERATOR "TBZ2")
SET(CPACK_INSTALL_CMAKE_PROJECTS  ${CMAKE_CURRENT_BINARY_DIR};FGRun;ALL;/)


# split version string into components, note CMAKE_MATCH_0 is the entire regexp match
string(REGEX MATCH "([0-9]+)\\.([0-9]+)" CPACK_PACKAGE_VERSION ${FGRUN_VERSION} )
set(CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_MATCH_1})
set(CPACK_PACKAGE_VERSION_MINOR ${CMAKE_MATCH_2})

message(STATUS "version is ${CPACK_PACKAGE_VERSION_MAJOR} dot ${CPACK_PACKAGE_VERSION_MINOR}")

set(CPACK_SOURCE_GENERATOR TBZ2)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "fgrun-${FGRUN_VERSION}" CACHE INTERNAL "tarball basename")
set(CPACK_SOURCE_IGNORE_FILES
  "\\\\.svn;Makefile.am;~$;${CPACK_SOURCE_IGNORE_FILES}")

message(STATUS "ignoring: ${CPACK_SOURCE_IGNORE_FILES}")

include (CPack)

option(ENABLE_NLS "Set to ON to build fgrun with NLS support" ON)
option(SIMGEAR_SHARED "Set to ON to dynamically link SimGear" OFF)
option(SHARED_FLTK "Set to ON to dynamically link FLTK (requires that SimGear also be dynamically linked)" OFF)
if (SHARED_FLTK AND NOT SIMGEAR_SHARED)
   message(FATAL_ERROR "Please do not use SHARED_FLTK without SIMGEAR_SHARED, as it will probably fail to compile")
endif (SHARED_FLTK AND NOT SIMGEAR_SHARED)

if (MSVC)
    GET_FILENAME_COMPONENT(PARENT_DIR ${PROJECT_SOURCE_DIR} PATH)
    if (CMAKE_CL_64)
        SET(TEST_3RDPARTY_DIR "${PARENT_DIR}/3rdparty.x64")
    else (CMAKE_CL_64)
        SET(TEST_3RDPARTY_DIR "${PARENT_DIR}/3rdparty")
    endif (CMAKE_CL_64)
    if (EXISTS ${TEST_3RDPARTY_DIR})
        set(MSVC_3RDPARTY_ROOT ${PARENT_DIR} CACHE PATH "Location where the third-party dependencies are extracted")
    else (EXISTS ${TEST_3RDPARTY_DIR})
        set(MSVC_3RDPARTY_ROOT NOT_FOUND CACHE PATH "Location where the third-party dependencies are extracted")
    endif (EXISTS ${TEST_3RDPARTY_DIR})
    list(APPEND PLATFORM_LIBS "winmm.lib")
else (MSVC)
    set(MSVC_3RDPARTY_ROOT NOT_FOUND CACHE PATH "Location where the third-party dependencies are extracted")
endif (MSVC)

if (MSVC AND MSVC_3RDPARTY_ROOT)

  message(STATUS "3rdparty files located in ${MSVC_3RDPARTY_ROOT}")
  set( OSG_MSVC "msvc" )
  if (${MSVC_VERSION} EQUAL 1700)
      set( OSG_MSVC ${OSG_MSVC}110 )
  elseif (${MSVC_VERSION} EQUAL 1600)
      set( OSG_MSVC ${OSG_MSVC}100 )
  else (${MSVC_VERSION} EQUAL 1700)
      set( OSG_MSVC ${OSG_MSVC}90 )
  endif (${MSVC_VERSION} EQUAL 1700)
  if (CMAKE_CL_64)
      set( OSG_MSVC ${OSG_MSVC}-64 )
      set( MSVC_3RDPARTY_DIR 3rdParty.x64 )
  else (CMAKE_CL_64)
      set( MSVC_3RDPARTY_DIR 3rdParty )
  endif (CMAKE_CL_64)
  find_path(FLTK_DIR include/FL/Fl.H
			${MSVC_3RDPARTY_ROOT}/install/${OSG_MSVC}/fltk
			${MSVC_3RDPARTY_ROOT}/install/${OSG_MSVC}/fltk-1.3
			${MSVC_3RDPARTY_ROOT}/${MSVC_3RDPARTY_DIR}
			)
  list (APPEND CMAKE_PROGRAM_PATH ${FLTK_DIR}/bin)

  set (CMAKE_LIBRARY_PATH ${FLTK_DIR}/lib ${MSVC_3RDPARTY_ROOT}/${MSVC_3RDPARTY_DIR}/lib ${MSVC_3RDPARTY_ROOT}/install/${OSG_MSVC}/OpenScenegraph/lib ${MSVC_3RDPARTY_ROOT}/install/${OSG_MSVC}/SimGear/lib )
  set (CMAKE_INCLUDE_PATH ${FLTK_DIR}/include ${MSVC_3RDPARTY_ROOT}/${MSVC_3RDPARTY_DIR}/include ${MSVC_3RDPARTY_ROOT}/install/${OSG_MSVC}/OpenScenegraph/include ${MSVC_3RDPARTY_ROOT}/install/${OSG_MSVC}/SimGear/include)

  GET_FILENAME_COMPONENT(MSVC_ROOT_PARENT_DIR ${MSVC_3RDPARTY_ROOT} PATH)
  find_path(BOOST_ROOT boost/version.hpp
      ${MSVC_ROOT_PARENT_DIR}
			${MSVC_3RDPARTY_ROOT}/boost
			${MSVC_3RDPARTY_ROOT}/boost_1_51_0
			${MSVC_3RDPARTY_ROOT}/boost_1_50_0
			${MSVC_3RDPARTY_ROOT}/boost_1_49_0
			${MSVC_3RDPARTY_ROOT}/boost_1_48_0
			${MSVC_3RDPARTY_ROOT}/boost_1_47_0
			${MSVC_3RDPARTY_ROOT}/boost_1_46_1
			${MSVC_3RDPARTY_ROOT}/boost_1_46_0
			${MSVC_3RDPARTY_ROOT}/boost_1_45_0
			${MSVC_3RDPARTY_ROOT}/boost_1_44_0
			)
  message(STATUS "BOOST_ROOT is ${BOOST_ROOT}")

endif (MSVC AND MSVC_3RDPARTY_ROOT)

if(WIN32)

    if(MSVC)
        set(MSVC_FLAGS "-DNOMINMAX -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D__CRT_NONSTDC_NO_WARNINGS")
        if (${MSVC_VERSION} GREATER 1599)
          set( MSVC_LD_FLAGS "/FORCE:MULTIPLE" )
        endif (${MSVC_VERSION} GREATER 1599)
    endif(MSVC)

    set(NOMINMAX 1)
    set( WINSOCK_LIBRARY "ws2_32.lib" )

endif(WIN32)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LD_FLAGS}")

find_package(Boost REQUIRED)
find_package(ZLIB REQUIRED)
if (SHARED_FLTK)
    #see http://sources.debian.net/src/fltk1.3/1.3.2-4/README.CMake.txt lines 200-240
    find_package(FLTK REQUIRED NO_MODULE)
    include(${FLTK_USE_FILE})
    set(FLTK_LIBRARIES "fltk_forms_SHARED;fltk_gl_SHARED;fltk_images_SHARED;fltk_SHARED")
else (SHARED_FLTK)
    find_package(FLTK REQUIRED)
endif (SHARED_FLTK)
find_package(OpenSceneGraph 2.8.1 REQUIRED osgText osgDB osgParticle osgUtil osgViewer osgGA)
find_package(SimGear ${FGRUN_VERSION} REQUIRED)
if (ENABLE_NLS)
    find_package(Gettext)
endif (ENABLE_NLS)

if (ENABLE_NLS AND GETTEXT_FOUND)

    if (MSVC)
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

        if (NOT INTL_LIBRARY)
            set( ENABLE_NLS 0 )
        endif (NOT INTL_LIBRARY)
    endif (MSVC)

	if (ENABLE_NLS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_NLS")
	endif (ENABLE_NLS)

else (ENABLE_NLS AND GETTEXT_FOUND)

    set( INTL_LIBRARY "" )

endif (ENABLE_NLS AND GETTEXT_FOUND)

check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(sys/wait.h HAVE_SYS_WAIT_H)
check_include_file(termios.h HAVE_TERMIOS_H)
check_include_file(pty.h HAVE_PTY_H)
check_include_file(utmp.h HAVE_UTMP_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(string.h HAVE_STRING_H)

check_function_exists(openpty HAVE_OPENPTY)
if (NOT HAVE_OPENPTY)
    unset( HAVE_OPENPTY CACHE )
    set( CMAKE_REQUIRED_LIBRARIES util )
    check_function_exists(openpty HAVE_OPENPTY)
    if (HAVE_OPENPTY)
        set( OPENPTY_LIBRARY util CACHE INTERNAL "" )
    else (HAVE_OPENPTY)
        set( CMAKE_REQUIRED_LIBRARIES )
    endif (HAVE_OPENPTY)
endif (NOT HAVE_OPENPTY)
check_function_exists(login_tty HAVE_LOGIN_TTY)
check_function_exists(_getpty HAVE__GETPTY)

if ( NOT ( HAVE_OPENPTY OR HAVE__GETPTY ) )

    if (EXISTS /dev/ptmx)
        set(HAVE_DEV_PTMX)
    endif (EXISTS /dev/ptmx)

    if (EXISTS /dev/pts)
        set(HAVE_DEV_PTS)
    endif (EXISTS /dev/pts)

endif ( NOT ( HAVE_OPENPTY OR HAVE__GETPTY ) )

if ( FLTK_FOUND )
    if ( X11_Xinerama_FOUND )
        message(STATUS "Found X11_Xinerama...")
        list(APPEND FLTK_LIBRARIES ${X11_Xinerama_LIB})
    endif()

    if ( X11_Xft_FOUND )
        message(STATUS "Found X11_Xft...")
        list(APPEND FLTK_LIBRARIES ${X11_Xft_LIB})
    endif()

    if ( CMAKE_DL_LIBS )
       list(APPEND FLTK_LIBRARIES ${CMAKE_DL_LIBS})
    endif()

    set( CMAKE_REQUIRED_INCLUDES ${FLTK_INCLUDE_DIR} )
    set( CMAKE_REQUIRED_LIBRARIES ${FLTK_LIBRARIES} )
    message(STATUS "Using FLTK_LIBRARIES for fgrun: ${FLTK_LIBRARIES}")

    check_cxx_source_runs(
        "#include <FL/Fl.H>
        int main() {
            return Fl::version() >= 1.03 ? 0 : -1;
        }
        "
        HAVE_FLTK_1_3)

	if ( NOT HAVE_FLTK_1_3 )
		message( SEND_ERROR "FLTK 1.3 is required" )
	endif ( NOT HAVE_FLTK_1_3 )

endif ( FLTK_FOUND )

add_definitions(-DHAVE_CONFIG_H)

include_directories(${PROJECT_BINARY_DIR}/src)
include_directories(${CMAKE_HOME_DIRECTORY}/src)

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
