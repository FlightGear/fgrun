// wizard_funcs.cxx -- Wizard UI functions.
//
// Written by Bernie Bright, started Nov 2003.
//
// Copyright (c) 2003  Bernie Bright - bbright@users.sourceforge.net
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

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

#include <FL/Fl.H>
#include <FL/Fl_Preferences.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>

#include <simgear/props/props_io.hxx>
#include <simgear/structure/exception.hxx>
#include <plib/ul.h>

#include "wizard.h"
#include "advanced.h"
#include "logwin.h"

using std::string;
using std::vector;

extern string def_fg_exe;
extern string def_fg_root;
extern string def_fg_scenery;

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

static bool
is_valid_fg_scenery( const string& dir )
{
    if (!fl_filename_isdir( dir.c_str() ))
        return false;

    // TODO: more definitive tests?

    return true;
}

void
Wizard::init()
{
    static const int npages = 5;

    for (int i = 0; i < npages; ++i)
        page[i]->hide();

    logwin = new LogWindow( 640, 480, "Log Window" );

    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

    prefs.get( "fg_exe", buf, def_fg_exe.c_str(), buflen-1);
    fg_exe_->value( buf );

    prefs.get( "fg_root", buf, def_fg_root.c_str(), buflen-1);
    fg_root_->value( buf );

    if (prefs.entryExists( "fg_scenery" ))
    {
	prefs.get( "fg_scenery", buf, "", buflen-1);
	fg_scenery_->value( buf );
    }
    else if (!def_fg_scenery.empty())
    {
        fg_scenery_->value( def_fg_scenery.c_str() );
    }
    else if (fg_root_->size() > 0)
    {
        string dir( fg_root_->value() );
        dir += "/Scenery";
        fg_scenery_->value( dir.c_str() );
    }

    if (fg_exe_->size() == 0 ||
	fg_root_->size() == 0 ||
        !is_valid_fg_root( fg_root_->value() ) ||
        fg_scenery_->size() == 0 ||
        !is_valid_fg_scenery( fg_scenery_->value() ))
    {
        // First time through or FG_ROOT is not valid.
        page[0]->activate();
        page[0]->show();
        prev->deactivate();
    }
    else
    {
        page[1]->show();
        prev->activate();
        aircraft_update();
        airports_->init( fg_root_->value(), fg_scenery_->value() );
    }

    next->deactivate();
}

void
Wizard::show()
{
    win->show();
}

void
Wizard::show( int ac, char* av[] )
{
    win->show( ac, av );
}

static const double update_period = 0.05;

static void
timeout_handler( void* v )
{
    ((Wizard*)v)->update_preview();
    Fl::repeat_timeout( update_period, timeout_handler, v );
}

void
Wizard::preview_aircraft()
{
    Fl::remove_timeout( timeout_handler, this );

    preview->clear();
    preview->init();

    int n = aircraft->value();
    if (n == 0)
        return;

    string ac( aircraft->text(n) );
    string fname( (char*)aircraft->data(n) );

    SGPropertyNode props;
    try
    {
        readProperties( fname, &props );

        if (props.hasValue( "/sim/model/path" ))
        {
            string mpath( fg_root_->value() );
            mpath.append( "/" );
            mpath.append( props.getStringValue( "/sim/model/path" ) );
            if (!ulFileExists( mpath.c_str() ))
	    {
		fl_alert( "Model not found: '%s'", mpath.c_str() );
                return;
	    }

            if (mpath.find( ".xml", mpath.size() - 4 ) != string::npos)
            {
                SGPropertyNode mprops;
                readProperties( mpath, &mprops );

                if (mprops.hasValue( "/path" ))
                {
                    string::size_type pos = mpath.rfind( "/" );
                    mpath.replace( pos+1, string::npos,
                                   mprops.getStringValue("/path") );
                }
            }


            win->cursor( FL_CURSOR_WAIT );
	    Fl::flush();
            ssgEntity* model = preview->load( mpath );
            if (model != 0)
            {
                Fl::add_timeout( update_period, timeout_handler, this );
            }
            win->cursor( FL_CURSOR_DEFAULT );
            preview->redraw();
        }
    }
    catch (const sg_exception& exc )
    {
        fl_alert( exc.getFormattedMessage().c_str() );
        return;
    }

    next->activate();
}

void
Wizard::next_cb()
{
    Fl::remove_timeout( timeout_handler, this );
    prev->activate();

    if (wiz->value() == page[0])
    {
	char abs_name[ FL_PATH_MAX ];

	fl_filename_absolute( abs_name, fg_exe_->value() );
	prefs.set( "fg_exe", abs_name );

	fl_filename_absolute( abs_name, fg_root_->value() );
	prefs.set( "fg_root", abs_name );

	fl_filename_absolute( abs_name, fg_scenery_->value() );
	prefs.set( "fg_scenery", abs_name );
    }
    else if (wiz->value() == page[1])
    {
	int n = aircraft->value();
	prefs.set( "aircraft", aircraft->text(n) );
    }
    else if (wiz->value() == page[2])
    {
	prefs.set( "airport", airports_->get_selected_id().c_str() );
	prefs.set( "airport-name", airports_->get_selected_name().c_str() );

	string rwy( airports_->get_selected_runway() );
	if (rwy.empty())
	    rwy = "<default>";
	prefs.set( "runway", rwy.c_str() );
    }
    else if (wiz->value() == page[3])
    {
	prefs.flush();

	if (write_fgfsrc())
	{
	    std::cout << "launching flightgear\n";
	    run_fgfs();
	}
	return;
    }

    wiz->next();

    if (wiz->value() == page[1])
    {
	// "Select aircraft" page
        if (aircraft->size() == 0)
            aircraft_update();
    }
    else if (wiz->value() == page[2])
    {
	// "Select location" page
    }
    else if (wiz->value() == page[3])
    {
	std::ostringstream ostr;
	ostr << fg_exe_->value() << "\n  ";
	write_fgfsrc( ostr, "\n  " );
	text->value( ostr.str().c_str() );
    }
}

void
Wizard::prev_cb()
{
    next->activate();
    wiz->prev();
    if (wiz->value() == page[0])
    {
        prev->deactivate();
    }
}

#if defined(WIN32) || defined(__EMX__) && !defined(__CYGWIN__)
static inline bool isdirsep(char c) {return c=='/' || c=='\\';}
#else
static inline bool isdirsep(char c) { return c =='/'; }
#endif

void
Wizard::fg_exe_select_cb()
{
    char* p = fl_file_chooser( "Select FlightGear executable",
                              fg_exe_->value(), 0);
    if (p != 0)
        fg_exe_->value( p );

    fg_exe_update_cb();
}

void
Wizard::fg_exe_update_cb()
{
//     if (fg_exe_->size() == 0)
// 	return;
}

void
Wizard::fg_root_update_cb()
{
    next->deactivate();

    if (fg_root_->size() == 0)
        return;

    string dir( fg_root_->value() );

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

    fg_root_->value( dir.c_str() );

    // Derive FG_SCENERY from FG_ROOT. 
    if (fg_scenery_->size() == 0)
    {
	string d( dir );
	d.append( "/Scenery" );
	if (!is_valid_fg_scenery( d ))
	    return;

	fg_scenery_->value( d.c_str() );
        //airports_->init( fg_root_->value(), fg_scenery_->value() );
    }

    next->activate();
}

void
Wizard::fg_root_select_cb()
{
    char* p = fl_dir_chooser( "Select FG_ROOT directory",
                              fg_root_->value(), 0);
    if (p != 0)
        fg_root_->value( p );

    fg_root_update_cb();
}

void
Wizard::fg_scenery_update_cb()
{
    next->deactivate();

    if (fg_scenery_->size() == 0)
        return;

    string dir( fg_scenery_->value() );

    // Remove trailing separator.
    if (isdirsep( dir[ dir.length() - 1 ] ))
    {
        dir.erase( dir.length() - 1 );
    }

    if (!is_valid_fg_scenery( dir ))
	return;

    fg_scenery_->value( dir.c_str() );
    next->activate();
}

void
Wizard::fg_scenery_select_cb()
{
    char* p = fl_dir_chooser( "Select FG_SCENERY directory",
                              fg_scenery_->value(), 0);
    if (p != 0)
        fg_scenery_->value( p );

    fg_scenery_update_cb();
}

void
Wizard::advanced_cb()
{
    static Advanced* adv = 0;

    if (adv == 0)
    {
	adv = new Advanced;
    }

    {
	prefs.set( "airport", airports_->get_selected_id().c_str() );
	prefs.set( "airport-name", airports_->get_selected_name().c_str() );
    }

    int r = adv->exec( prefs );
}

void
Wizard::update_preview()
{
    preview->update();
}

static void
search_aircraft_dir( const string& dir,
                     bool recursive,
                     vector<string>& ac )
{
    dirent** files;
    int num_files = fl_filename_list( dir.c_str(),
                                      &files, fl_casenumericsort );
    if (num_files < 0)
        return;

    for (int i = 0; i < num_files; ++i)
    {
        if (fl_filename_match(files[i]->d_name, "*-set.xml"))
        {
            string d( dir );
            d.append( "/" );
            d.append( files[i]->d_name );
            ac.push_back( d );

        }
        else if (recursive &&
                 strcmp( files[i]->d_name, "CVS" ) != 0 &&
                 strcmp( files[i]->d_name, ".." ) != 0 &&
                 strcmp( files[i]->d_name, "." ) != 0 )
        {
            string d( dir );
            d.append( "/" );
            d.append( files[i]->d_name );
            if (fl_filename_isdir( d.c_str() ))
            {
                search_aircraft_dir( d.c_str(), false, ac );
            }
        }

        free( files[i] );
    }
    free( files );
}

void
Wizard::aircraft_update()
{
    string s( fg_root_->value() );
    s.append( "/Aircraft/" );
    vector< string > ac;
    search_aircraft_dir( s, true, ac );

    for (int i = 1; i <= aircraft->size(); ++i)
    {
        char* data = (char*)aircraft->data(i);
        if (data != 0)
            free( data );
    }
    aircraft->clear();

    for (vector<string>::size_type i = 0; i < ac.size(); ++i)
    {
        // Extract aircraft name from filename.
        string::size_type pos = ac[i].rfind( "/" );
        string::size_type epos = ac[i].find( "-set.xml", pos );

        char* data = strdup( ac[i].c_str() );
        aircraft->add( ac[i].substr( pos+1, epos-pos-1 ).c_str(), data );
    }
}

Wizard::~Wizard()
{
    delete logwin;
    delete win;
}
