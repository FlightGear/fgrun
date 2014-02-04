// main.cxx -- FlightGear Launch Control
//
// Written by Bernie Bright, started Oct 2002.
//
// Copyright (c) 2002  Bernie Bright - bbright@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// $Id$

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string>
#include <cstring>
#include <FL/Fl.H>
#include <FL/filename.H>

#include <osgDB/Registry>
#include <simgear/scene/model/ModelRegistry.hxx>
#include <simgear/io/raw_socket.hxx>
#include <simgear/structure/OSGVersion.hxx>

#include "wizard.h"
#include "i18n.h"

std::string def_fg_exe = "";
std::string def_fg_root = "";
std::string def_fg_scenery = "";
std::string def_ts_exe = "";
std::string version = "";
std::string def_ts_dir = "";
std::string def_acft_dir = "";
static bool silent = false;
static bool fullscreen = false;

/**
 * --fg-exe=<PATH>
 * --fg-root=<DIR>
 * --fg-scenery=<DIR>
 */
static int
parse_args( int, char** argv, int& i )
{
    if (strcmp( argv[i], "--silent" ) == 0)
    {
        silent = true;
        ++i;
        return 1;
    }
    else if (strncmp( argv[i], "--fg-exe=", 9 ) == 0)
    {
        def_fg_exe.assign( &argv[i][9] );
        ++i;
        return 1;
    }
    else if (strncmp( argv[i], "--fg-root=", 10 ) == 0)
    {
        def_fg_root.assign( &argv[i][10] );
        def_fg_scenery = def_fg_root;
        def_fg_scenery += "/Scenery";

        ++i;
        return 1;
    }
    else if (strncmp( argv[i], "--fg-scenery=", 13 ) == 0)
    {
        def_fg_scenery.assign( &argv[i][13] );
        ++i;
        return 1;
    }
    else if (strcmp( argv[i], "--fullscreen" ) == 0 ||
             strcmp( argv[i], "-f" ) == 0)
    {
        fullscreen = true;
        ++i;
        return 1;
    }
    else if (strncmp( argv[i], "--terrasync-dir=", 16 ) == 0)
    {
        def_ts_dir.assign( &argv[i][16] );
        ++i;
        return 1;
    }
    else if (strncmp( argv[i], "--fg-aircraft=", 14 ) == 0)
    {
        def_acft_dir.assign( &argv[i][14] );
        ++i;
        return 1;
    }	
    else if (strncmp( argv[i], "--version=", 10 ) == 0)
    {
        version.assign( &argv[i][10] );
        ++i;
        return 1;
    }
    return 0;
}

string
get_locale_directory( const char *argv0 )
{
#ifdef LOCALEDIR
    SGPath localedir( LOCALEDIR );
    if (localedir.exists())
        return localedir.str();
#endif

    SGPath path( argv0 );
    path = path.dir();

    if (path.file() == "bin")
    {
        SGPath path2 = path.dir();
        path2.append( "share/locale" );
        if (path2.exists() )
            return path2.str();
    }
    path.append( "locale" );
    return path.str();
}

int
main( int argc, char* argv[] )
{
    //_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    simgear::Socket::initSockets();
    setlocale( LC_ALL, "" );
    string localedir = get_locale_directory( argv[0] );
    bindtextdomain( PACKAGE, localedir.c_str() );
    textdomain( PACKAGE );

    osgDB::Registry* registry = osgDB::Registry::instance();
#if ( SG_OSG_VERSION >= 28003 && SG_OSG_VERSION < 29000 ) || SG_OSG_VERSION >= 29006
    registry->setExpiryDelay( 0. );
#endif
    osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options;
    //int cacheOptions = osgDB::ReaderWriter::Options::CACHE_ALL;
    //options->setObjectCacheHint((osgDB::ReaderWriter::Options::CacheHintOptions)cacheOptions);
    registry->setOptions(options);

    // Link with the ModelRegistry class to install the AC replacement loader
    simgear::ModelRegistry::instance();

    Fl::lock(); // initialize multithreading

    int i = 0;
    if (Fl::args( argc, argv, i, parse_args ) < argc)
    {
        Fl::fatal(_("Options are:\n --silent\n --fg-exe=<PATH>\n --fg-root=<DIR>\n --fg-scenery=<DIR>\n --terrasync-dir=<DIR>\n --fg-aircraft=<DIR>\n --version=<VERSION>\n -f, --fullscreen\n%s"), Fl::help );
    }

    if ( silent )
    {
        Fl_Preferences prefs( Fl_Preferences::SYSTEM, "flightgear.org", "fgrun" );
        char abs_name[ FL_PATH_MAX ];

        if ( !def_fg_exe.empty() )
        {
            fl_filename_absolute( abs_name, def_fg_exe.c_str() );
            prefs.set( "fg_exe_init", abs_name );
            prefs.set( "fg_exe", abs_name );
        }

        if ( !def_fg_root.empty() )
        {
            fl_filename_absolute( abs_name, def_fg_root.c_str() );
            prefs.set( "fg_root_init", abs_name );
            prefs.set( "fg_root", abs_name );
        }

        if ( !def_fg_scenery.empty() )
        {
            fl_filename_absolute( abs_name, def_fg_scenery.c_str() );
            prefs.set( "fg_scenery_init", abs_name );
            prefs.set( "fg_scenery", abs_name );
        }

        if ( !def_ts_dir.empty() )
        {
            fl_filename_absolute( abs_name, def_ts_dir.c_str() );
            prefs.set( "ts_dir_init", abs_name );
            prefs.set( "ts_dir", abs_name );
        }
		
        if ( !def_acft_dir.empty() )
        {
            fl_filename_absolute( abs_name, def_acft_dir.c_str() );
            prefs.set( "fg_aircraft_init", abs_name );
            prefs.set( "fg_aircraft", abs_name );
        }

        if ( !version.empty() )
        {
            int major, minor, patch;
            char c1, c2;
            std::istringstream iss(version);
            iss >> major >> c1 >> minor >> c2 >> patch;
            if (c1 == '.' && c2 == '.') {
                prefs.set("version", major * 10000 + minor * 100 + patch);
            }
        }
        return 0;
    }

    Fl::visual( FL_DOUBLE | FL_INDEX );

    Wizard ui;
    ui.init( fullscreen );

    ui.show();

    return Fl::run();
}
