// init.cxx -- Iinitialization and support functions.
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

#include <fstream>
#include <string>

#include <FL/filename.h>
#include <FL/Fl_File_Chooser.h>

#include "UserInterface.h"
#include "airportdb.h"

void load_airportdb_cb( void* );

void
UserInterface::init()
{
    airportdb_ = new AirportDB;

    page_list->add("General");
    page_list->add("Features");
    page_list->add("Flight Model");
    page_list->add("Freeze");
    page_list->add("Initial Position");
    page_list->add("Rendering");
    page_list->add("Time");
    page_list->add("Network");
    page_list->add("Input/Output");
    page_list->add("Avioncs");
    page_list->add("Properties");
    page_list->add("Debugging");
    page_list->add("Environment");
    page_list->add("Airport Browser");
    page_list->select(1);
    show_page(0);

    load_settings_cb();

    if (fg_root->size() > 0
	&& fl_filename_isdir(fg_root->value()))
    {
	Fl::add_idle(update_aircraft_cb, this);
    }

    update_airports_cb();
}

void
UserInterface::fg_exe_cb()
{
    char* p = fl_file_chooser("Select executable", 0, fg_exe->value(), 0);
    if (p != 0)
    {
	fg_exe->value( p );
	run->activate();
    }
}

void
UserInterface::fg_root_cb()
{
    char* p = fl_dir_chooser( "Select FG_ROOT directory",
			      fg_root->value(), 0);
    if (p == 0)
	return;

    fg_root->value( p );
    fg_root_update_cb();
}

void
UserInterface::fg_scenery_cb()
{
    char* p = fl_dir_chooser("Select FG_SCENERY directory",
			     fg_scenery->value(), 0);
    if (p != 0)
    {
	fg_scenery->value( p );
	airport_update->activate();
	aircraft_update->activate();
    }
}

void
UserInterface::fg_browser_cb()
{
    char* p = fl_file_chooser("Select browser", 0, browser->value(), 0);
    if (p != 0)
	browser->value( p );
}

using std::string;

#if defined(WIN32) || defined(__EMX__) && !defined(__CYGWIN__)
static inline bool isdirsep(char c) {return c=='/' || c=='\\';}
#else
static inline bool isdirsep(char c) { return c =='/'; }
#endif

static bool
is_valid_fg_root( const string& dir )
{
    if (!fl_filename_isdir( dir.c_str() ))
	return false;

    string fname( dir );
    fname.append( "/version" );
    std::ifstream file( fname.c_str() );
    return file.is_open();
}

void
UserInterface::fg_root_update_cb()
{ 
    airport_update->deactivate();
    aircraft_update->deactivate();

    if (fg_root->size() == 0)
	return;

    string dir( fg_root->value() );

    // Remove trailing separator.
    if (isdirsep( dir[ dir.length() - 1 ] ))
    {
	dir.erase( dir.length() - 1 );
    }

    if (!is_valid_fg_root( dir ))
    {
	dir.append( "/data" );
	if (!is_valid_fg_root( dir ))
	    return;
    }

    fg_root->value( dir.c_str() );
    dir.append( "/Scenery" );
    fg_scenery->value( dir.c_str() );

    airport_update->activate();
    aircraft_update->activate();
}

void
UserInterface::fg_scenery_update_cb()
{
}
