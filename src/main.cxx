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
#include <FL/Fl.H>
#include <FL/filename.H>

#include <osgDB/Registry>
#include <simgear/scene/model/ModelRegistry.hxx>
#include <plib/netSocket.h>

#include "wizard.h"
#include "i18n.h"

std::string def_fg_exe = "";
std::string def_fg_root = "";
std::string def_fg_scenery = "";
std::string def_ts_exe = "";
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
    else if (strncmp( argv[i], "--ts-exe=", 9 ) == 0)
    {
	def_ts_exe.assign( &argv[i][9] );
	++i;
	return 1;
    }
    return 0;
}

string
get_locale_directory( const char *argv0 )
{
#if _MSC_VER
    SGPath path = argv0;
    path = path.dir();
    path.append( "locale" );
    return path.str();
#else
    return LOCALEDIR;
#endif
}

int
main( int argc, char* argv[] )
{
    netInit( &argc,argv );
    setlocale( LC_ALL, "" );
    string localedir = get_locale_directory( argv[0] );
    bindtextdomain( PACKAGE, localedir.c_str() );
    textdomain( PACKAGE );

    osgDB::Registry* registry = osgDB::Registry::instance();
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
        Fl::fatal(_("Options are:\n --silent\n --fg-exe=<PATH>\n --fg-root=<DIR>\n --fg-scenery=<DIR>\n --ts-exe=<PATH>\n -f, --fullscreen\n%s"), Fl::help );
    }

    if ( silent )
    {
        Fl_Preferences prefs( Fl_Preferences::USER, "flightgear.org", "fgrun" );
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

	if ( !def_ts_exe.empty() )
	{
	    fl_filename_absolute( abs_name, def_ts_exe.c_str() );
	    prefs.set( "ts_exe_init", abs_name );
	    prefs.set( "ts_exe", abs_name );
	}

	return 0;
    }

    Fl::visual( FL_DOUBLE | FL_INDEX );

    Wizard ui;
    ui.init( fullscreen );

    ui.show();

    return Fl::run();
}
