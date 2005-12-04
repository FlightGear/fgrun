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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <string>
#include <cstdio>
#include <vector>
#include <sstream>

#include <FL/Fl.H>
#include <FL/Fl_Preferences.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

#include <simgear/props/props_io.hxx>
#include <simgear/structure/exception.hxx>
#include <simgear/misc/sg_path.hxx>

#include <plib/ul.h>

#include "wizard.h"
#include "advanced.h"
#include "logwin.h"

#if defined(WIN32) || defined(__EMX__) && !defined(__CYGWIN__)
# include "os_win32.h"
#else
# include "os_posix.h"
#endif

using std::string;
using std::vector;

extern string def_fg_exe;
extern string def_fg_root;
extern string def_fg_scenery;

/**
 * Data associated with each entry in the aircraft browser.
 */
struct AircraftData
{
    SGPropertyNode props;
    std::string name;
};

static bool
is_valid_fg_root( const string& dir )
{
    if (!fl_filename_isdir( dir.c_str() ))
        return false;

    string fname( dir );
    fname.append( "/version" );
    FILE* fp = fopen( fname.c_str(), "r" );
    if (fp != 0)
    {
	fclose(fp);
	return true;
    }

    return false;
}

void
Wizard::airports_cb( Fl_Widget*, void* v )
{
    ((Wizard*)v)->airports_cb();
}

void
Wizard::airports_cb()
{
    if (airports_->loaded())
    {
	airports_->show_installed( true );
	cache_delete_->activate();

	const int buflen = FL_PATH_MAX;
	char buf[ buflen ];

  	if (prefs.get( "airport", buf, "", buflen-1) && buf[0] != 0)
	{
	    airports_->select_id( buf );
	}
    }
}

static const char* about_text = "\
<html>\
 <head>\
  <title>FlightGear Launch Control " VERSION "</title>\
 </head>\
 <body>\
  <h1>FlightGear Launch Control " VERSION "</h1>\
  <p>This program is released under the GNU General Public License (http://www.gnu.org/copyleft/gpl.html).</p>\
  <p>Report bugs to http://sourceforge.net/projects/fgrun</p>\
 </body>\
</html>";

void
Wizard::init( bool fullscreen )
{
    static const int npages = 5;

    make_launch_window();

    for (int i = 0; i < npages; ++i)
        page[i]->hide();

    if (fullscreen)
    {
	win->resize( 0, 0, Fl::w(), Fl::h() );
    }
    else
    {
	int X, Y, W, H;
	prefs.get( "x", X, 0 );
	prefs.get( "y", Y, 0 );
	prefs.get( "width", W, 0 );
	prefs.get( "height", H, 0 );
	if (W > 0 && H > 0)
	    win->resize( X, Y, W, H );
    }

    win->size_range( 640, 480 );
    text->buffer( new Fl_Text_Buffer );

    logwin = new LogWindow( 640, 480, "Log Window" );

    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

    prefs.getUserdataPath( buf, sizeof(buf) );
    SGPath cache( buf );
    cache.append( "airports.txt" );
    cache_file_->value( cache.c_str() );

    about_->value( about_text );

    prefs.get( "fg_exe", buf, def_fg_exe.c_str(), buflen-1);
    fg_exe_->value( buf );

    prefs.get( "fg_root", buf, def_fg_root.c_str(), buflen-1);
    fg_root_->value( buf );

    string fg_scenery;
    if (!def_fg_scenery.empty())
    {
	fg_scenery = def_fg_scenery;
    }
    else if (prefs.get( "fg_scenery", buf, "", buflen-1))
    {
	fg_scenery = buf;
    }
    else if (fg_root_->size() > 0)
    {
	fg_scenery = fg_root_->value();
	fg_scenery += "/Scenery";
    }

    typedef vector<string> vs_t;
    vs_t v( sgPathSplit( fg_scenery ) );
    for (vs_t::size_type i = 0; i < v.size(); ++i)
	scenery_dir_list_->add( v[i].c_str() );

    if (fg_exe_->size() == 0 ||
	fg_root_->size() == 0 ||
        !is_valid_fg_root( fg_root_->value() ) ||
	fg_scenery.empty() )
    {
        // First time through or FG_ROOT is not valid.
        page[0]->activate();
        page[0]->show();
        prev->deactivate();
	next->deactivate();
    }
    else
    {
	airports_->set_refresh_callback( refresh_airports, this );
	refresh_airports();
        aircraft_update();

        prev->activate();
	next->activate();
        page[1]->show();
    }

    int iVal;
    prefs.get("show_cmd_line", iVal, 0);
    show_cmd_line->value(iVal);
    if ( iVal )
	text->show();
    else
	text->hide();
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

static SGPath
dir_path( const SGPath& p )
{
    return SGPath( p.dir() );
}

/**
 * Locate a named SSG node in a branch.
 */
static ssgEntity *
find_named_node (ssgEntity * node, const char * name)
{
  char * node_name = node->getName();
  if (node_name != 0 && !strcmp(name, node_name))
    return node;
  else if (node->isAKindOf(ssgTypeBranch())) {
    int nKids = node->getNumKids();
    for (int i = 0; i < nKids; i++) {
      ssgEntity * result =
        find_named_node(((ssgBranch*)node)->getKid(i), name);
      if (result != 0)
        return result;
    }
  } 
  return 0;
}

void
Wizard::preview_aircraft()
{
    Fl::remove_timeout( timeout_handler, this );

    preview->make_current();

    preview->clear();
    preview->redraw();
    preview->init();

    int n = aircraft->value();
    if (n == 0)
        return;

    AircraftData* data =
	reinterpret_cast<AircraftData*>( aircraft->data(n) );

    if (data->props.hasValue( "/sim/model/path" ))
    {
	SGPath path( fg_root_->value() ), tpath;
	path.append( data->props.getStringValue( "/sim/model/path" ) );

	if (!path.exists())
	{
	    fl_alert( "Model not found: '%s'", path.c_str() );
	    return;
	}

	try
	{
	    if (path.extension() == "xml")
            {
                SGPropertyNode mprops;
                readProperties( path.str(), &mprops );

                tpath = dir_path( path );
                if (mprops.hasValue( "path" ))
                {
		    path = dir_path( path );
		    path.append( mprops.getStringValue("path") );
                }

                if (mprops.hasValue( "texture-path" ))
                    tpath.append( mprops.getStringValue("texture-path") );
            }

            win->cursor( FL_CURSOR_WAIT );
	    Fl::flush();

	    ssgEntity* model = preview->load( path.str(), tpath.str() );
	    if (model != 0)
	    {
                ssgEntity *bounding_obj = find_named_node( model, "Aircraft" );
                preview->set_model( model, bounding_obj );

		Fl::add_timeout( update_period, timeout_handler, this );
	    }
            win->cursor( FL_CURSOR_DEFAULT );
            preview->redraw();
	}
	catch (const sg_exception& exc )
	{
	    fl_alert( exc.getFormattedMessage().c_str() );
	    return;
	}
	catch (...)
	{}
    }
    else
    {
	fl_alert( "Property '/sim/model/path' not found" );
	return;
    }

    next->activate();
}

void
Wizard::next_cb()
{
    prev->activate();

    if (wiz->value() == page[0])
    {
	char abs_name[ FL_PATH_MAX ];

	fl_filename_absolute( abs_name, fg_exe_->value() );
	prefs.set( "fg_exe", abs_name );

	fl_filename_absolute( abs_name, fg_root_->value() );
	prefs.set( "fg_root", abs_name );

	string fg_scenery = scenery_dir_list_->text(1);
	for (int i = 2; i <= scenery_dir_list_->size(); ++i)
	{
	    fg_scenery += os::searchPathSep;
	    fg_scenery += scenery_dir_list_->text(i);
	}

 	prefs.set( "fg_scenery", fg_scenery.c_str() );

	refresh_airports();
    }
    else if (wiz->value() == page[1])
    {
	int n = aircraft->value();
	if (n > 0)
	{
	    AircraftData* data =
		reinterpret_cast<AircraftData*>( aircraft->data(n) );
	    prefs.set( "aircraft", n > 0 ? data->name.c_str() : "" );
	}
	Fl::remove_timeout( timeout_handler, this );
    }
    else if (wiz->value() == page[2])
    {
	prefs.set( "airport", airports_->get_selected_id().c_str() );
	prefs.set( "airport-name",
		   airports_->get_selected_name().c_str() );

	string rwy( airports_->get_selected_runway() );
	if (rwy.empty())
	    rwy = "<default>";
	prefs.set( "runway", rwy.c_str() );

	update_basic_options();
    }
    else if (wiz->value() == page[3])
    {
	prefs.flush();

	int err = pthread_create( &th, 0, &Wizard::startFlightGear_cb, (void *)this );

	exec_launch_window();
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
	if (!airports_->loaded())
	{
 	    win->cursor( FL_CURSOR_WAIT );
	    Fl::flush();
	}
    }
    else if (wiz->value() == page[3])
    {
	std::ostringstream ostr;
	ostr << fg_exe_->value() << "\n  ";
	write_fgfsrc( ostr, "\n  " );
	text->buffer()->text( ostr.str().c_str() );
	next->label( "Run" );
    }
}

void
Wizard::prev_cb()
{
    next->activate();
    next->label( "Next" );
    wiz->prev();
    if (wiz->value() == page[0])
    {
        prev->deactivate();
    }
}

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
    if (os::isdirsep( dir[ dir.length() - 1 ] ))
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

    if (scenery_dir_list_->size() == 0)
    {
	// Derive FG_SCENERY from FG_ROOT. 
	string d( dir );
	d.append( "/Scenery" );
	if (!fl_filename_isdir( d.c_str() ))
	    return;

	scenery_dir_list_->add( d.c_str() );
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
Wizard::advanced_cb()
{
    static Advanced* adv = 0;

    if (adv == 0)
    {
	adv = new Advanced;
    }

    prefs.set( "airport", airports_->get_selected_id().c_str() );
    prefs.set( "airport-name", airports_->get_selected_name().c_str() );

    int r = adv->exec( prefs );
    if (r)
    {
    }

    update_basic_options();

    // Update command text.
    std::ostringstream ostr;
    ostr << fg_exe_->value() << "\n  ";
    write_fgfsrc( ostr, "\n  " );
    text->buffer()->text( ostr.str().c_str() );
}

void
Wizard::update_preview()
{
    preview->update();
}

static void
search_aircraft_dir( const SGPath& dir,
                     bool recursive,
                     vector<SGPath>& ac )
{
    dirent** files;
    string s( dir.str() );

#ifdef WIN32
    // Ensure there is a trailing slash.
    if (*s.rbegin() != '/')
	s.append( "/" );
#endif

    int num_files = fl_filename_list( s.c_str(),
                                      &files, fl_casenumericsort );
    if (num_files < 0)
        return;

    for (int i = 0; i < num_files; ++i)
    {
        if (fl_filename_match(files[i]->d_name, "*-set.xml"))
        {
            SGPath d( dir );
            d.append( files[i]->d_name );
            ac.push_back( d );
        }
        else if (recursive &&
                 strcmp( files[i]->d_name, "CVS" ) != 0 &&
                 strcmp( files[i]->d_name, ".." ) != 0 &&
                 strcmp( files[i]->d_name, "." ) != 0 )
        {
            SGPath d( dir );
            d.append( files[i]->d_name );
            if (fl_filename_isdir( d.c_str() ))
            {
                search_aircraft_dir( d, false, ac );
            }
        }

        free( files[i] );
    }
    free( files );
}

static void
delayed_preview( void* v )
{
    ((Wizard*)v)->preview_aircraft();
}

void
Wizard::aircraft_update()
{
    SGPath path( fg_root_->value() );
    path.append( "Aircraft" );
    vector< SGPath > ac;
    search_aircraft_dir( path, true, ac );

    // Empty the aircraft browser list.
    for (int i = 1; i <= aircraft->size(); ++i)
    {
	AircraftData* data =
	    reinterpret_cast<AircraftData*>( aircraft->data(i) );
	delete data;
    }
    aircraft->clear();

    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];
    prefs.get( "aircraft", buf, "", buflen-1);

    bool selected = false;
    // Populate the aircraft browser list.
    for (vector<SGPath>::size_type vi = 0; vi < ac.size(); ++vi)
    {
	string s( ac[vi].str() );

	SGPropertyNode props;
	try
	{
	    readProperties( s.c_str(), &props );
	}
	catch (const sg_exception&)
	{
	    continue;
	}

        if (props.hasValue( "/sim/description" ))
        {
            string desc = props.getStringValue( "/sim/description" );

            if ( desc.find( "Alias " ) == string::npos )
            {
		// Extract aircraft name from filename.
                string::size_type pos = s.rfind( "/" );
                string::size_type epos = s.find( "-set.xml", pos );
	        string ss( s.substr( pos+1, epos-pos-1 ) );

		AircraftData* data = new AircraftData;
		copyProperties( &props, &data->props );
		//data->props = props;
		data->name = ss;
                aircraft->add( desc.c_str(), data );

	        if (buf[0] != 0 && strcmp( buf, ss.c_str() ) == 0)
	        {
	             aircraft->select( aircraft->size() );
                     selected = true;
	        }
            }
        }
    }

    if ( selected )
        Fl::add_timeout( 0.1, delayed_preview, this );
}

Wizard::~Wizard()
{
    delete logwin;
    delete win;
}

void
Wizard::cancel_cb()
{
    logwin->hide();

    // Save main window size and position.
    prefs.set( "x", win->x() );
    prefs.set( "y", win->y() );
    prefs.set( "width", win->w() );
    prefs.set( "height", win->h() );

    win->hide();
}

void
Wizard::delete_cache_file_cb()
{
    char buf[ FL_PATH_MAX ];
    prefs.getUserdataPath( buf, sizeof(buf) );
    SGPath path( buf );
    path.append( "/airports.txt" );

    if (!path.exists())
	return;

    //TODO: win32 support.
    if (unlink( path.c_str() ) == 0)
	return;

    fl_alert( "Unable to delete '%s':\n%s",
	      path.c_str(), strerror(errno) );
}

void
Wizard::scenery_dir_select_cb()
{
    scenery_dir_up_->deactivate();
    scenery_dir_down_->deactivate();
    int n = scenery_dir_list_->value();
    if (n > 0)
    {
	scenery_dir_delete_->activate();
	if (n > 1)
	    scenery_dir_up_->activate();
	if (n < scenery_dir_list_->size())
	    scenery_dir_down_->activate();

	scenery_dir_list_->deselect();
	scenery_dir_list_->select( n );
    }
    else
    {
	scenery_dir_delete_->deactivate();
    }
}

void
Wizard::scenery_dir_add_cb()
{
    char* p = fl_dir_chooser( "Select FG_SCENERY directory", 0, 0);
    if (p != 0)
    {
	scenery_dir_list_->add( p );
	scenery_dir_list_->value( scenery_dir_list_->size() );
	scenery_dir_delete_->activate();
	scenery_dir_list_->select( scenery_dir_list_->size() );
    }
}

void
Wizard::scenery_dir_delete_cb()
{
    int n = scenery_dir_list_->value();
    if (n > 0)
	scenery_dir_list_->remove( n );

    if (scenery_dir_list_->size() == 0)
	scenery_dir_delete_->deactivate();
}

/**
 * Move a directory entry up the list.
 */
void
Wizard::scenery_dir_up_cb()
{
    int from = scenery_dir_list_->value();
    int to = from - 1;
    scenery_dir_list_->move( to, from );

    scenery_dir_list_->deselect();
    scenery_dir_list_->select( to );

    scenery_dir_down_->activate();
    if (to == 1)
	scenery_dir_up_->deactivate();
}

/**
 * Move a directory entry down the list.
 */
void
Wizard::scenery_dir_down_cb()
{
    int n = scenery_dir_list_->value();
    scenery_dir_list_->insert( n+2, scenery_dir_list_->text(n) );
    scenery_dir_list_->remove( n );

    scenery_dir_list_->deselect();
    scenery_dir_list_->select( n+1 );

    scenery_dir_up_->activate();
    if (n+1 == scenery_dir_list_->size())
	scenery_dir_down_->deactivate();
}

/**
 * Force a re-scan of the scenery directories for new airports.
 */
void
Wizard::refresh_airports( Fl_Widget*, void* v )
{
    static_cast<Wizard*>(v)->delete_cache_file_cb();
    static_cast<Wizard*>(v)->refresh_airports();
}


void
Wizard::refresh_airports()
{
//     win->cursor( FL_CURSOR_WAIT );

    vector<string> v;
    for (int i = 1; i <= scenery_dir_list_->size(); ++i)
    {
	SGPath dir( scenery_dir_list_->text(i) );
	dir.append( "Terrain" );
	if (fl_filename_isdir( dir.c_str() ))
	    v.push_back( dir.str() );
	else
	    v.push_back( scenery_dir_list_->text(i) );
    }

    SGPath path( fg_root_->value() );
    path.append( "/Airports/apt.dat.gz" );
    airports_->load_runways( path.str(), airports_cb, this );

    string cache( cache_file_->value() );
    airports_->load_airports( v, cache, airports_cb, this );
}

void
Wizard::update_options()
{
    // Update command text.
    std::ostringstream ostr;
    ostr << fg_exe_->value() << "\n  ";
    write_fgfsrc( ostr, "\n  " );
    text->buffer()->text( ostr.str().c_str() );
}

void
Wizard::resolution_cb()
{
    prefs.set("geometry", resolution->text());
    update_options();
}

void
Wizard::game_mode_cb()
{
    prefs.set("game_mode", game_mode->value());
    update_options();
}

void
Wizard::horizon_effect_cb()
{
    prefs.set("horizon_effect", horizon_effect->value());
    update_options();
}

void
Wizard::enhanced_lighting_cb()
{
    prefs.set("enhanced_lighting", enhanced_lighting->value());
    update_options();
}

void
Wizard::specular_highlight_cb()
{
    prefs.set("specular_highlight", specular_highlight->value());
    update_options();
}

void
Wizard::clouds_3d_cb()
{
    prefs.set("clouds3d", clouds_3d->value());
    update_options();
}

void
Wizard::random_objects_cb()
{
    prefs.set("random_objects", random_objects->value());
    update_options();
}

void
Wizard::ai_models_cb()
{
    prefs.set("ai_models", ai_models->value());
    update_options();
}

void
Wizard::time_of_day_cb()
{
    int v = time_of_day->value();
    if ( v == 0 )
    {
	time_of_day_value->deactivate();
	prefs.set("time-match-real", 1);
    }
    else
    {
	time_of_day_value->activate();
	prefs.set("time-match-real", 0);
	prefs.set("time-match-local", 0);
	prefs.set("start-date-sys", 0);
	prefs.set("start-date-gmt", 0);
	prefs.set("start-date-lat", 0);
    }
    prefs.set("time_of_day", v);
    update_options();
}

void
Wizard::time_of_day_value_cb()
{
    prefs.set("time_of_day_value", time_of_day_value->text());
    update_options();
}

void
Wizard::real_weather_fetch_cb()
{
    prefs.set("fetch_real_weather", real_weather_fetch->value());
    update_options();
}

void
Wizard::auto_coordination_cb()
{
    prefs.set("auto_coordination", auto_coordination->value());
    update_options();
}

void
Wizard::atlas_cb()
{
    int v = atlas->value();
    if ( v == 0 )
    {
	atlas_host->deactivate();
	atlas_port->deactivate();
	std::vector<string> io_list;
	int i, iVal;
	prefs.get("io-count", iVal, 0);
	for ( i = 1; i <= iVal; ++i )
	{
	    char buf[256];
	    prefs.get( Fl_Preferences::Name( "io-item-%d", i ), buf, "", sizeof buf - 1 );
	    string item( buf );
	    if ( item.length() < 8 || item.substr( 0, 8 ) != "--atlas=" )
	    {
		io_list.push_back(buf);
	    }
	}
	prefs.set("io-count",(int)io_list.size());
	for ( i = 0; i < io_list.size(); ++i )
	{
	    prefs.set( Fl_Preferences::Name( "io-item-%d", i+1 ), io_list[i].c_str());
	}
    }
    else
    {
	atlas_host->activate();
	atlas_port->activate();
	int i, iVal, loc = 0;
	prefs.get("io-count", iVal, 0);
	for ( i = 1; i <= iVal; ++i )
	{
	    char buf[256];
	    prefs.get( Fl_Preferences::Name( "io-item-%d", i ), buf, "", sizeof buf - 1 );
	    string item( buf );
	    if ( item.length() > 8 && item.substr( 0, 8 ) == "--atlas=" )
	    {
		loc = i;
		break;
	    }
	}
	string host = atlas_host->value();
	if ( host.empty() )
	    host = "localhost";
	int port = (int)atlas_port->value();
	if ( port == 0 )
	    port = 5500;
	atlas_port->value(port);
	std::ostringstream opt;
	opt << "--atlas=socket,out,5," << host << "," << port << ",udp";
	if ( loc == 0 )
	{
	    loc = iVal + 1;
	    prefs.set("io-count",loc);
	}
	prefs.set( Fl_Preferences::Name( "io-item-%d", loc ), opt.str().c_str() );
    }
    update_options();
}

void
Wizard::atlas_host_cb()
{
    int i, iVal, loc = 0;
    prefs.get("io-count", iVal, 0);
    for ( i = 1; i <= iVal; ++i )
    {
	char buf[256];
	prefs.get( Fl_Preferences::Name( "io-item-%d", i ), buf, "", sizeof buf - 1 );
	string item( buf );
	if ( item.length() > 8 && item.substr( 0, 8 ) == "--atlas=" )
	{
	    loc = i;
	    break;
	}
    }
    string host = atlas_host->value();
    if ( host.empty() )
	host = "localhost";
    int port = (int)atlas_port->value();
    if ( port == 0 )
	port = 5500;
    atlas_port->value(port);
    std::ostringstream opt;
    opt << "--atlas=socket,out,5," << host << "," << port << ",udp";
    if ( loc == 0 )
    {
	loc = iVal + 1;
	prefs.set("io-count",loc);
    }
    prefs.set( Fl_Preferences::Name( "io-item-%d", loc ), opt.str().c_str() );
    update_options();
}

void
Wizard::atlas_port_cb()
{
    int i, iVal, loc = 0;
    prefs.get("io-count", iVal, 0);
    for ( i = 1; i <= iVal; ++i )
    {
	char buf[256];
	prefs.get( Fl_Preferences::Name( "io-item-%d", i ), buf, "", sizeof buf - 1 );
	string item( buf );
	if ( item.length() > 8 && item.substr( 0, 8 ) == "--atlas=" )
	{
	    loc = i;
	    break;
	}
    }
    string host = atlas_host->value();
    if ( host.empty() )
	host = "localhost";
    int port = (int)atlas_port->value();
    if ( port == 0 )
	port = 5500;
    std::ostringstream opt;
    opt << "--atlas=socket,out,5," << host << "," << port << ",udp";
    if ( loc == 0 )
    {
	loc = iVal + 1;
	prefs.set("io-count",loc);
    }
    prefs.set( Fl_Preferences::Name( "io-item-%d", loc ), opt.str().c_str() );
    update_options();
}

void
Wizard::multiplay_cb()
{
    int v = multiplay->value();
    if ( v == 0 )
    {
	multiplay_callsign->deactivate();
	multiplay_host->deactivate();
	multiplay_in->deactivate();
	multiplay_out->deactivate();
	prefs.set("callsign","");
	prefs.set("multiplay1","");
	prefs.set("multiplay2","");
    }
    else
    {
	multiplay_callsign->activate();
	multiplay_host->activate();
	multiplay_in->activate();
	multiplay_out->activate();

	string callsign = multiplay_callsign->value();
	string host = multiplay_host->value();
	int in = (int)multiplay_in->value();
	int out = (int)multiplay_out->value();

	prefs.set("callsign",callsign.c_str());
	std::ostringstream str;
	str << "out,10," << host << "," << out;
	prefs.set("multiplay1",str.str().c_str());
	str.str("");
	char hostname[256];
	gethostname( hostname, 256 );
	str << "in,10," << hostname << "," << in;
	prefs.set("multiplay2",str.str().c_str());
    }
    update_options();
}

void
Wizard::multiplay_callsign_cb()
{
    string callsign = multiplay_callsign->value();
    string host = multiplay_host->value();
    int in = (int)multiplay_in->value();
    int out = (int)multiplay_out->value();

    prefs.set("callsign",callsign.c_str());
    std::ostringstream str;
    str << "out,10," << host << "," << out;
    prefs.set("multiplay1",str.str().c_str());
    str.str("");
    str << "in,10,localhost," << in;
    prefs.set("multiplay2",str.str().c_str());
    update_options();
}

void
Wizard::multiplay_host_cb()
{
    string callsign = multiplay_callsign->value();
    string host = multiplay_host->value();
    int in = (int)multiplay_in->value();
    int out = (int)multiplay_out->value();

    prefs.set("callsign",callsign.c_str());
    std::ostringstream str;
    str << "out,10," << host << "," << out;
    prefs.set("multiplay1",str.str().c_str());
    str.str("");
    str << "in,10,localhost," << in;
    prefs.set("multiplay2",str.str().c_str());
    update_options();
}

void
Wizard::multiplay_in_cb()
{
    string callsign = multiplay_callsign->value();
    string host = multiplay_host->value();
    int in = (int)multiplay_in->value();
    int out = (int)multiplay_out->value();

    prefs.set("callsign",callsign.c_str());
    std::ostringstream str;
    str << "out,10," << host << "," << out;
    prefs.set("multiplay1",str.str().c_str());
    str.str("");
    str << "in,10,localhost," << in;
    prefs.set("multiplay2",str.str().c_str());
    update_options();
}

void
Wizard::multiplay_out_cb()
{
    string callsign = multiplay_callsign->value();
    string host = multiplay_host->value();
    int in = (int)multiplay_in->value();
    int out = (int)multiplay_out->value();

    prefs.set("callsign",callsign.c_str());
    std::ostringstream str;
    str << "out,10," << host << "," << out;
    prefs.set("multiplay1",str.str().c_str());
    str.str("");
    str << "in,10,localhost," << in;
    prefs.set("multiplay2",str.str().c_str());
    update_options();
}

void
Wizard::update_basic_options()
{
    const int buflen = 256;
    char buf[ buflen ];

    prefs.get("geometry", buf, "", buflen-1);
    int i;
    for ( i = 0; menu_resolution[i].text != 0; i++ )
    {
	if ( strcmp( buf, menu_resolution[i].text ) == 0 )
	{
	    resolution->value(i);
	    break;
	}
    }
    if ( menu_resolution[i].text == 0 )
    {
	const char *t = resolution->text(i);
	if ( t == 0 )
	    resolution->add( buf );
	else if ( strcmp( t, buf ) != 0 )
	{
	    resolution->remove(i);
	    resolution->add( buf );
	}
	resolution->value(i);
    }

    int iVal;
    prefs.get("game_mode", iVal, 0);
    game_mode->value(iVal);
    prefs.get("horizon_effect", iVal, 0);
    horizon_effect->value(iVal);
    prefs.get("enhanced_lighting", iVal, 0);
    enhanced_lighting->value(iVal);
    prefs.get("specular_highlight", iVal, 0);
    specular_highlight->value(iVal);
    prefs.get("clouds3d", iVal, 0);
    clouds_3d->value(iVal);
    prefs.get("random_objects", iVal, 0);
    random_objects->value(iVal);
    prefs.get("ai_models", iVal, 0);
    ai_models->value(iVal);
    prefs.get("time_of_day", iVal, 0);
    time_of_day->value(iVal);
    prefs.get("time_of_day_value", buf, "noon", buflen-1);
    time_of_day_value->value(0);
    for ( i = 0; menu_time_of_day_value[i].text != 0; i++ )
    {
	if ( strcmp( buf, menu_time_of_day_value[i].text ) == 0 )
	{
	    time_of_day_value->value(i);
	    break;
	}
    }
    prefs.get("fetch_real_weather", iVal, 0);
    real_weather_fetch->value(iVal);
    prefs.get("auto_coordination", iVal, 0);
    auto_coordination->value(iVal);

    atlas->value(0);
    atlas_host->value("");
    atlas_host->deactivate();
    atlas_port->value(0);
    atlas_port->deactivate();
    prefs.get("io-count", iVal, 0);
    for ( i = 1; i <= iVal; ++i )
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("io-item-%d", i), buf, "", buflen-1 );
	string item( buf );
	if ( item.length() > 19 && item.substr( 0, 19 ) == "--atlas=socket,out," )
	{
	    item.erase( 0, 19 );
	    string::size_type p = item.find( ',' );
	    if ( p != string::npos )
	    {
		item.erase( 0, p+1 );
		p = item.find( ',' );
		if ( p != string::npos )
		{
		    string host = item.substr(0, p);
		    item.erase( 0, p+1 );
		    p = item.find( ',' );
		    if ( p != string::npos )
		    {
			string port = item.substr(0, p);
			item.erase(0, p+1);
			if ( item == "udp" )
			{
			    atlas->value(1);
			    atlas_host->value( host.c_str() );
			    atlas_port->value( atoi( port.c_str() ) );
			    atlas_host->activate();
			    atlas_port->activate();
			}
		    }
		}
	    }
	}
    }

    multiplay->value(0);
    multiplay_callsign->value("");
    multiplay_host->value("");
    multiplay_in->value(0);
    multiplay_out->value(0);
    multiplay_callsign->deactivate();
    multiplay_host->deactivate();
    multiplay_in->deactivate();
    multiplay_out->deactivate();

    prefs.get("callsign", buf, "", buflen-1 );
    string callsign(buf);
    prefs.get("multiplay1", buf, "", buflen-1 );
    string multiplay1(buf);
    prefs.get("multiplay2", buf, "", buflen-1 );
    string multiplay2(buf);

    if ( callsign.size() && multiplay1.size() && multiplay2.size() )
    {
	if ( multiplay1.size() >= 3 && multiplay2.size() >= 3 )
	{
	    if ( multiplay1.substr(0,3) != "out" || multiplay2.substr(0,3) != "in," )
	    {
		string tmp = multiplay1;
		multiplay1 = multiplay2;
		multiplay2 = tmp;
	    }
	    if ( multiplay1.substr(0,3) == "out" && multiplay2.substr(0,3) == "in," )
	    {
		if ( multiplay1.size() >= 4 )
		{
		    multiplay1.erase(0, 4);
		    multiplay2.erase(0, 3);

		    string::size_type p = multiplay1.find(',');
		    if ( p != string::npos )
		    {
			multiplay1.erase(0, p+1);
			p = multiplay2.find(',');
			if ( p != string::npos )
			{
			    multiplay2.erase(0, p+1);
			    p = multiplay1.find(',');
			    if ( p != string::npos )
			    {
				string host = multiplay1.substr(0, p);
				multiplay1.erase(0, p+1);
				p = multiplay2.find(',');
				if ( p != string::npos )
				{
				    multiplay2.erase(0, p+1);
				    int out = atoi(multiplay1.c_str());
				    int in = atoi(multiplay2.c_str());

				    multiplay->value(1);
				    multiplay_callsign->value(callsign.c_str());
				    multiplay_callsign->activate();
				    multiplay_host->value(host.c_str());
				    multiplay_host->activate();
				    multiplay_in->value(in);
				    multiplay_in->activate();
				    multiplay_out->value(out);
				    multiplay_out->activate();
				}
			    }
			}
		    }
		}
	    }
	}
    }
}

void
Wizard::show_cmd_line_cb()
{
    if ( show_cmd_line->value() )
	text->show();
    else
	text->hide();
    prefs.set("show_cmd_line", show_cmd_line->value());
}

void *
Wizard::startFlightGear_cb( void *d )
{
    static_cast<Wizard *>( d )->startFlightGear_cb();
    return 0;
}

void
Wizard::startFlightGear_cb()
{
    std::ostringstream ostr;
    if (write_fgfsrc( ostr, " " ))
    {
	run_fgfs(ostr.str());
	launch_result = 0;
    }
}

void
Wizard::exec_launch_window()
{
    int X = win->x(),
        Y = win->y(),
        W = win->w(),
        H = win->h(),
        w = launch_window->w(),
        h = launch_window->h();
    launch_window->position( X + ( W - w ) / 2, Y + ( H - h ) / 2 );
    launch_result = -1;
    launch_window->set_modal();
    launch_window->show();
    while ( launch_result == -1 )
    {
	Fl::wait();
    }
    launch_window->set_non_modal();
    launch_window->hide();
}
