// FGRunUI.cxx -- FlightGear Run User Interface
//
// Written by Bernie Bright, started Oct 2002.
//
// Copyright (C) 2002  Bernie Bright - bbright@bigpond.net.au
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

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <FL/Fl_Preferences.h>
#include <FL/Fl_File_Chooser.h>
#include <FL/Fl_Text_Buffer.h>
#include <FL/filename.h>
#ifdef HAVE_STRING_H
#  include <string.h>
#endif
#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <sys/wait.h>
#include <fcntl.h>
#include "FGRunUI.h"

using std::cout;
using std::cerr;
using std::vector;
using std::ofstream;

FGRunUI::FGRunUI()
    : UserInterface()
      , default_aircraft("c172")
      , default_airport("KSFO")
      , modflag(false)
      , io_options_list_value(0)
{
    // Ensure the "General" tab is displayed.
    tabs->value( general_tab );
    output_text->buffer( new Fl_Text_Buffer );
}

FGRunUI::~FGRunUI()
{
}

/**
 * 
 */
static int
set_choice( Fl_Choice* w, const char* s )
{
    const Fl_Menu_Item* m = w->menu();
    int i = 0;
    int nest = 0;
    for (i = 0; i < m->size(); ++i)
    {
	if (m[i].submenu())
	{
	    ++nest;
	}
	else if (m[i].label() != 0)
	{
	    if (strcmp( s, m[i].label() ) == 0)
	    {
		w->value( i );
		return 1;
	    }
	}
	else
	{
	    --nest;
	}
    }
    return 0;
}


void
FGRunUI::load_settings()
{
    const int buflen = 1024;
    char buf[ buflen ];
    Fl_Preferences app( Fl_Preferences::USER,
			"flightgear.org", "fgrun/preferences" );
    Fl_Preferences prefs( app, "Settings" );

    // 
    prefs.get( "fg_exe", buf, "", buflen-1 );
    fg_exe->value( buf );
#if defined(WIN32)
    prefs.get( "fg_root", buf, "\\FlightGear", buflen-1 );
#else
    prefs.get( "fg_root", buf, "/usr/local/lib/FlightGear", buflen-1 );
#endif
    if (fl_filename_isdir( buf ))
    {
	fg_root->value( buf );
    }

#if defined(WIN32)
    prefs.get( "fg_scenery", buf, "\\FlightGear\\Scenery", buflen-1 );
#else
    prefs.get( "fg_scenery", buf, "/usr/local/lib/FlightGear/Scenery",
	       buflen-1 );
#endif
    if (fl_filename_isdir( buf ))
    {
	fg_scenery->value( buf );
    }

    prefs.get( "aircraft", buf, "c172", buflen-1 );
    default_aircraft = buf;
    prefs.get( "airport", buf, "KSFO", buflen-1 );
    default_airport = buf;

    int bVal;
    int iVal;
    double dVal;

    // General tab.
    prefs.get( "game_mode", bVal, 0 );
    enable_game_mode->value( bVal );
    prefs.get( "splash_screen", bVal, 1 );
    enable_splash_screen->value( bVal );
    prefs.get( "intro_music", bVal, 1 );
    enable_intro_music->value( bVal );
    prefs.get( "mouse_pointer", bVal, 1 );
    enable_mouse_pointer->value( bVal );
    prefs.get( "random_objects", bVal, 0 );
    enable_random_objects->value( bVal );

    prefs.get( "freeze", bVal, 0 );
    freeze->value( bVal );
    prefs.get( "fuel_freeze", bVal, 0 );
    fuel_freeze->value( bVal );
    prefs.get( "clock_freeze", bVal, 0 );
    clock_freeze->value( bVal );
    prefs.get( "auto_coordination", bVal, 0 );
    auto_coordination->value( bVal );

//     prefs.get( "units", buf, "feet", buflen-1 );
//     if (strcmp( buf, "feet" ) == 0)
// 	units_feet->setonly();
//     else
// 	units_meters->setonly();

#if defined(WIN32)
    prefs.get( "browser", buf, "webrun.bat", buflen-1 );
#else
    prefs.get( "browser", buf, "netscape", buflen-1 );
#endif
    browser->value( buf );
    prefs.get( "control", buf, "joystick", buflen-1 );
    set_choice( control, buf );

    // Features tab.
    prefs.get( "hud", bVal, 0 );
    enable_hud->value( bVal );
    prefs.get( "antialias_hud", bVal, 0 );
    enable_antialias_hud->value( bVal );
    enable_hud->do_callback();
    prefs.get( "panel", bVal, 1 );
    enable_panel->value( bVal );
    prefs.get( "sound", bVal, 1 );
    enable_sound->value( bVal );

    // Rendering tab.
    prefs.get( "clouds", bVal, 1 );
    enable_clouds->value( bVal );
    prefs.get( "clouds3d", bVal, 0 );
    enable_clouds3d->value( bVal );
    prefs.get( "skyblend", bVal, 1 );
    enable_skyblend->value( bVal );
    prefs.get( "textures", bVal, 1 );
    enable_textures->value( bVal );
    prefs.get( "wireframe", bVal, 0 );
    enable_wireframe->value( bVal );
    prefs.get( "fullscreen", bVal, 0 );
    enable_fullscreen->value( bVal );

    prefs.get( "fog", buf, "nice", buflen-1 );
    set_choice( fog, buf );

    prefs.get( "shading-flat", bVal, 0 );
    shading_flat->value( bVal );

    prefs.get( "fov", dVal, 60.0 );
    fov->value( fov->clamp( dVal ) );

    // Flight Model tab.
    prefs.get( "fdm", buf, "jsb", buflen-1 );
    set_choice( fdm, buf );
    fdm->do_callback();
    prefs.get( "model_hz", iVal, 120 );
    model_hz->value( model_hz->clamp( iVal ) );
    prefs.get( "speed", iVal, 1 );
    speed->value( speed->clamp( iVal ) );
    prefs.get( "notrim", bVal, 0 );
    notrim->value( bVal );
    prefs.get( "on_ground", bVal, 0 );
    on_ground->value( bVal );
    prefs.get( "in_air", bVal, 0 );
    in_air->value( bVal );

    prefs.get( "lon", buf, "", buflen-1 );
    lon->value( buf );
    prefs.get( "lat", buf, "", buflen-1 );
    lat->value( buf );
    prefs.get( "altitude", buf, "", buflen-1 );
    altitude->value( buf );
    prefs.get( "heading", buf, "", buflen-1 );
    heading->value( buf );
    prefs.get( "roll", buf, "", buflen-1 );
    roll->value( buf );
    prefs.get( "pitch", buf, "", buflen-1 );
    pitch->value( buf );
    prefs.get( "vc", buf, "", buflen-1 );
    vc->value( buf );

    // Time tab.
    prefs.get( "time-offset", bVal, 0 );
    time_offset->value( bVal );
    prefs.get( "time-offset-value", buf, "", buflen-1);
    time_offset_text->value( buf );
    if (bVal)
	time_offset_text->activate();
    else
	time_offset_text->deactivate();

    prefs.get( "time-match-real", bVal, 1 );
    time_match_real->value( bVal );

    prefs.get( "time-match-local", bVal, 0 );
    time_match_local->value( bVal );

    prefs.get( "start-date-sys", bVal, 0 );
    start_date_sys->value( bVal );
    prefs.get( "start-date-sys-value", buf, "", buflen-1);
    start_date_sys_text->value( buf );

    prefs.get( "start-date-lat", bVal, 0 );
    start_date_lat->value( bVal );
    prefs.get( "start-date-lat-value", buf, "", buflen-1);
    start_date_lat_text->value( buf );

    prefs.get( "start-date-gmt", bVal, 0 );
    start_date_gmt->value( bVal );
    prefs.get( "start-date-gmt-value", buf, "", buflen-1);
    start_date_gmt_text->value( buf );

    // I/O tab.
    // Network tab.
}

void
FGRunUI::save_settings()
{
    Fl_Preferences app( Fl_Preferences::USER,
			"flightgear.org", "fgrun/preferences" );
    Fl_Preferences prefs( app, "Settings" );

    prefs.set( "fg_exe", fg_exe->value() );
    prefs.set( "fg_root", fg_root->value() );
    prefs.set( "fg_scenery", fg_scenery->value() );
    prefs.set( "aircraft", aircraft->text() );
    prefs.set( "airport", airport->text() );
    prefs.set( "browser", browser->value() );

    // General tab.

    prefs.set( "game_mode", int(enable_game_mode->value()) );
    prefs.set( "splash_screen", int(enable_splash_screen->value()) );
    prefs.set( "intro_music", int(enable_intro_music->value()) );
    prefs.set( "mouse_pointer", int(enable_mouse_pointer->value()) );
    prefs.set( "random_objects", int(enable_random_objects->value()) );

    prefs.set( "freeze", int(freeze->value()) );
    prefs.set( "fuel_freeze", int(fuel_freeze->value()) );
    prefs.set( "clock_freeze", int(clock_freeze->value()) );
    prefs.set( "auto_coordination", int(auto_coordination->value()) );

//     if (units_feet->value())
// 	prefs.set( "units", "feet" );
//     else
// 	prefs.set( "units", "meters" );

    prefs.set( "control", control->text() );

    // Features tab.
    prefs.set( "hud", int(enable_hud->value()) );
    prefs.set( "antialias_hud", int(enable_antialias_hud->value()) );
    prefs.set( "panel", int(enable_panel->value()) );

    // Rendering tab.
    prefs.set( "clouds", int(enable_clouds->value()) );
    prefs.set( "clouds3d", int(enable_clouds3d->value()) );
    prefs.set( "skyblend", int(enable_skyblend->value()) );
    prefs.set( "textures", int(enable_textures->value()) );
    prefs.set( "wireframe", int(enable_wireframe->value()) );
    prefs.set( "fullscreen", int(enable_fullscreen->value()) );
    prefs.set( "sound", int(enable_sound->value()) );
    prefs.set( "fog", fog->text() );

    prefs.set( "shading-flat", shading_flat->value() );

    prefs.set( "fov", fov->value() );

    // Flight Model tab.
    prefs.set( "fdm", fdm->text() );
    prefs.set( "model_hz", int(model_hz->value()) );
    prefs.set( "speed", int(speed->value()) );
    prefs.set( "notrim", notrim->value() );
    prefs.set( "on_ground", on_ground->value() );
    prefs.set( "in_air", in_air->value() );
    prefs.set( "lon", lon->value() );
    prefs.set( "lat", lat->value() );
    prefs.set( "altitude", altitude->value() );
    prefs.set( "heading", heading->value() );
    prefs.set( "roll", roll->value() );
    prefs.set( "pitch", pitch->value() );
    prefs.set( "vc", vc->value() );
 
    // Time tab.
    prefs.set( "time-offset", time_offset->value());
    prefs.set( "time-offset-value", time_offset_text->value());
    prefs.set( "time-match-real", time_match_real->value());
    prefs.set( "time-match-local", time_match_local->value());
    prefs.set( "start-date-sys", start_date_sys->value());
    prefs.set( "start-date-sys-value", start_date_sys_text->value());
    prefs.set( "start-date-lat", start_date_lat->value());
    prefs.set( "start-date-lat-value", start_date_lat_text->value());
    prefs.set( "start-date-gmt", start_date_gmt->value());
    prefs.set( "start-date-gmt-value", start_date_gmt_text->value());

    // I/O tab.
    prefs.set( "io_options_count", io_options_list->size() );
    int i;
    for (i = 1; i < io_options_list->size(); ++i)
    {
	prefs.set( Fl_Preferences::Name("io%d", i), io_options_list->text(i));
    }

    // Network tab.

    modflag = false;
}

void
FGRunUI::update_aircraft()
{
    aircraft->clear();
    cout << "Searching for aircraft...\n";
    string acdir = fg_root->value();
    acdir += "/Aircraft";

    dirent **files;
    int num_files = fl_filename_list( acdir.c_str(), &files, fl_numericsort );
    if (num_files < 0)
    {
	return;
    }

    for (int i = 0; i < num_files; ++i)
    {
	if (fl_filename_match( files[i]->d_name, "*-set.xml"))
	{
	    // Extract aircraft name from filename.
	    char* p = strstr( files[i]->d_name, "-set.xml" );
	    if (p != 0)
	    {
		*p = 0;
	    }

	    int index = aircraft->add( files[i]->d_name, 0, 0, 0, 0 );

	    // Select the default aircraft
	    if (strcmp(files[i]->d_name, default_aircraft.c_str() ) == 0)
	    {
		aircraft->value(index);
	    }
	}
	free( files[i] );
    }

    free( files );
}

/**
 * Recursively search through scenery directories looking for airport files.
 */
static void
scan_for_airports( const char* scenery, vector<string>& apts )
{
    dirent **files;
    int num_files = fl_filename_list( scenery, &files, fl_numericsort );
    if (num_files < 0)
	return;

    for (int i = 0; i < num_files; ++i)
    {
	if (fl_filename_match( files[i]->d_name,
			       "[ew][0-9][0-9][0-9][ns][0-9][0-9]"))
	{
	    string dir = scenery;
	    dir += "/";
	    dir += files[i]->d_name;
	    if (fl_filename_isdir( dir.c_str() ))
	    {
		scan_for_airports( dir.c_str(), apts );
	    }
	}
	else if (fl_filename_match( files[i]->d_name, "???.btg.gz") ||
		 fl_filename_match( files[i]->d_name, "????.btg.gz")
		 )
	{
	    char* p = strstr( files[i]->d_name, ".btg");
	    if (p != 0)
		*p = 0;
	    apts.push_back( string( files[i]->d_name ) );
	}
	free( files[i] );
    }

    free( files );
}

/**
 * Functor to add a airports to a Fl_Choice widget.
 */
struct AddAirportHelper
{
    Fl_Choice* airport;
    AddAirportHelper( Fl_Choice* apt ) : airport(apt) {}

    void operator()( const string& s ) {
	string x(s, 0, 1);
	x += "/";
	x += s;
	airport->add( x.c_str(), 0, 0, 0, 0 );
    }
};

/**
 * 
 */
void
FGRunUI::update_airports()
{
    airport->clear();
    cout << "Searching for airports...\n";
    vector< string > apts;
    scan_for_airports( fg_scenery->value(), apts );
    std::sort( apts.begin(), apts.end() );
    std::for_each( apts.begin(), apts.end(), AddAirportHelper( airport ) );
    set_choice( airport, default_airport.c_str() );
}

/**
 * 
 */
void
FGRunUI::run_fgfs()
{
    char fname[ FL_PATH_MAX ];
    fl_filename_expand( fname, "~/.fgfsrc" );
    ofstream ofs( fname );
    if (ofs)
    {
	int i;

	ofs << "--fg-root=" << fg_root->value()
	    << "\n--fg-scenery=" << fg_scenery->value()
	    << "\n--airport-id=" << airport->text()
	    << "\n--aircraft=" << aircraft->text();

	// Only write non-default options.

// 	if (units_meters->value())
// 	    ofs << "\n--unit-meters";

	if (enable_game_mode->value())
	    ofs << "\n--enable-game-mode";

	if (!enable_splash_screen->value())
	    ofs << "\n--disable-splash-screen";

	if (!enable_intro_music->value())
	    ofs << "\n--disable-intro-music";

// 	if (enable_mouse_pointer->value())
// 	    ofs << "\n--enable-mouse-pointer";
// 	else
// 	    ofs << "\n--disable-mouse-pointer";

	if (enable_random_objects->value())
	    ofs << "\n--enable-random-objects";
	else
	    ofs << "\n--disable-random-objects";

	if (enable_clouds->value() == 0)
	    ofs << "\n--disable-clouds";

	if (enable_clouds3d->value())
	    ofs << "\n--enable-clouds3d";
	else
	    ofs << "\n--disable-clouds3d";

	if (!enable_sound->value())
	    ofs << "\n--disable-sound";

	if (strcmp( fog->text(), "disabled" ) == 0)
	    ofs << "\n--fog-disable";
	else if (strcmp( fog->text(), "fast" ) == 0)
	    ofs << "\n--fog-fastest";

	if (shading_flat->value())
	    ofs << "\n--shading-flat";

	if (!enable_panel->value())
	    ofs << "\n--disable-panel";

	if (enable_hud->value())
	    ofs << "\n--enable-hud";

// 	if (enable_antialias_hud->value())
// 	    ofs << "\n--enable-anti-alias-hud";
// 	else
// 	    ofs << "\n--disable-anti-alias-hud";

	if (!enable_skyblend->value())
	    ofs << "\n--disable-skyblend";

	if (enable_wireframe->value())
	    ofs << "\n--enable-wireframe";

	ofs << "\n--fdm=" << fdm->text();
	if (strcmp( fdm->text(), "jsb") == 0 && notrim->value())
	    ofs << "\n--notrim";
	if (in_air->value())
	    ofs << "\n--in-air";

	if (time_offset->value())
	    ofs << "\n--time-offset=" << time_offset_text->value();
	else if (time_match_real->value())
	    ofs << "\n--time-match-real";
	else if (time_match_local->value())
	    ofs << "\n--time-match-local";
	else if (start_date_sys->value())
	    ofs << "\n--start-date-sys=" << start_date_sys_text->value();
	else if (start_date_lat->value())
	    ofs << "\n--start-date-lat=" << start_date_lat_text->value();
	else if (start_date_gmt->value())
	    ofs << "\n--start-date-gmt=" << start_date_gmt_text->value();

	if (strcmp( control->text(), "joystick" ) != 0)
	    ofs << "\n--control=" << control->text();

	if (lon->value()[0] != 0)
	    ofs << "\n--lon=" << lon->value();
	if (lat->value()[0] != 0)
	    ofs << "\n--lat=" << lat->value();
	if (altitude->value()[0] != 0)
	    ofs << "\n--altitude=" << altitude->value();
	if (heading->value()[0] != 0)
	    ofs << "\n--heading=" << heading->value();
	if (roll->value()[0] != 0)
	    ofs << "\n--roll=" << roll->value();
	if (pitch->value()[0] != 0)
	    ofs << "\n--pitch=" << pitch->value();
	if (vc->value()[0] != 0)
	    ofs << "\n--vc=" << vc->value();

	if (httpd->value())
	    ofs << "\n--httpd=" << httpd_port->value();
	if (props->value())
	    ofs << "\n--props=" << props_port->value();
	if (jpg_httpd->value())
	    ofs << "\n--jpg-httpd=" << jpg_httpd_port->value();

	for (i = 1; i <= io_options_list->size(); ++i)
	{
	    ofs << "\n" << io_options_list->text(i);
	}

	for (i = 1; i <= props_list->size(); ++i)
	{
	    ofs << "\n" << props_list->text(i);
	}

	ofs << "\n";

	ofs.close();
    }

    int fd1[2];
    int fd2[2];
    if (pipe( fd1 ) < 0 || pipe( fd2 ) < 0)
    {
	perror( "pipe error" );
	return;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
	perror( "fork error" );
	return;
    }
    else if (pid > 0)
    {
	// parent

	// Close write-end of pipes.
 	close( fd1[1] );
 	close( fd2[1] );

	// Establish callbacks to read from pipes.
	Fl::add_fd( fd1[0], FL_READ, stdout_cb, (void*)this );
	Fl::add_fd( fd2[0], FL_READ, stderr_cb, (void*)this );

	output_text->buffer()->remove( 0, output_text->buffer()->length() );
	output_window->show();
    }
    else
    {
	// child

	// Close the read end of pipes.
	close( fd1[0] );
	close( fd2[0] );

	// Redirect stdout to write end of pipe 1.
	close(1);
	if (dup( fd1[1] ) < 0)
	    perror( "dup(fd1[1] error");

	// Redirect stderr to write end of pipe 2.
	close(2);
	if (dup( fd2[1] ) < 0)
	    perror( "dup(fd2[1] error");

	string path = fg_exe->value();
	string arg0;
	string::size_type idx = path.find_last_of( "/\\" );
	if (idx != string::npos)
	{
	    arg0 = path.substr( idx+1 );
	}
	else
	{
	    arg0 = path;
	}

	execl( path.c_str(), arg0.c_str(), NULL );
    }
}

void
FGRunUI::stdout_cb( int fd, void* p )
{
    if (p != 0)
	((FGRunUI*)p)->stdout_cb_i( fd );
}

void
FGRunUI::stderr_cb( int fd, void* p )
{
    if (p != 0)
	((FGRunUI*)p)->stderr_cb_i( fd );
}


void
FGRunUI::stdout_cb_i( int fd )
{
    char buf[200];
    int r = read( fd, buf, sizeof(buf)-1 );
    if (r > 0)
    {
	buf[r] = 0;
	//fputs( buf, stdout );
    }
    else
    {
	Fl::remove_fd( fd );
	close( fd );
	int status;
	waitpid( -1, &status, WNOHANG );
	return;
    }
}

void
FGRunUI::stderr_cb_i( int fd )
{
    char buf[200];
    int r = read( fd, buf, sizeof(buf)-1 );
    if (r > 0)
    {
	buf[r] = 0;
	//output_text->buffer()->append( buf );
	output_text->insert( buf );
	output_text->show_insert_position();
    }
    else
    {
	Fl::remove_fd( fd );
	close( fd );
	int status;
	waitpid( -1, &status, WNOHANG );
	return;
    }
}
void
FGRunUI::select_fg_exe()
{
    char* p = fl_file_chooser( "Select fgfs executable", NULL, NULL, 0);
    if (p != 0)
    {
	fg_exe->value( p );
	modflag = true;
    }
}

void
FGRunUI::select_fg_root()
{
    char* p = fl_dir_chooser( "Select FG_ROOT directory",
			      fg_root->value(), 0 );
    if (p != 0)
    {
	fg_root->value( p );

	if (fg_scenery->value()[0] == 0)
	{
	    char dir[1024];
	    snprintf( dir, sizeof(dir), "%s/Scenery", p );
	    fg_scenery->value( dir );
	}

	update_aircraft();
	update_airports();
	modflag = true;
    }
}

void
FGRunUI::select_fg_scenery()
{
    char* p = fl_dir_chooser( "Select FG_SCENERY directory",
			      fg_scenery->value(), 0 );
    if (p != 0)
    {
	fg_scenery->value( p );
	update_airports();
	modflag = true;
    }
}

void
FGRunUI::select_browser()
{
    char* p = fl_file_chooser( "Select FlightGear help browser",
			      browser->value(), 0 );
    if (p != 0)
    {
	browser->value( p );
	modflag = true;
    }
}

void
FGRunUI::add_io_item()
{
    io_options_list_value = 0;
    addOptionDlg->show();
    io_medium->do_callback();
}

void
FGRunUI::edit_io_item()
{
    io_options_list_value = io_options_list->value();
    if (io_options_list_value <= 0)
	return;

    const char* item = io_options_list->text( io_options_list_value );
    char protocol[32];
    char medium[32];
    char direction[32];
    int hz = 0;
    char rem[80];

    int n = sscanf( item, "--%[^=]=%[^,],%[^,],%d,%s",
		    protocol, medium, direction, &hz, rem );
    set_choice( io_protocol, protocol );
    set_choice( io_medium, medium );
    io_medium->do_callback();
    io_hz->value( double(hz) );
    set_choice( io_dir, direction );

    if (strcmp( medium, "file" ) == 0)
    {
	file_name->value( rem );
    }
    else if (strcmp( medium, "serial" ) == 0)
    {
	char port[32];
	char baud[32];
	sscanf( rem, "%[^,],%s", port, baud );
	serial_port->value( port );
	serial_baud->value( baud );
    }
    else if (strcmp( medium, "socket" ) == 0)
    {
	char hostname[32];
	int port;
	char type[32];
	sscanf( rem, "%[^,],%d,%s", hostname, &port, type );
    }

    addOptionDlg->show();
}

void
FGRunUI::delete_io_item()
{
    int i = io_options_list->value();
    if (i <= 0)
	return;

    io_options_list->remove( i );
}

void
FGRunUI::OptionDlg_ok_cb()
{
    char buf[256];
    char buf2[256];

    snprintf( buf2, sizeof(buf2), "--%s=%s,%s,%d",
	      io_protocol->text(),
	      io_medium->text(),
	      io_dir->text(),
	      int(io_hz->value()) );

    if (strcmp(io_medium->text(), "file") == 0)
    {
	snprintf( buf, sizeof(buf), "%s,%s",
		  buf2, file_name->value());
    }
    else if (strcmp(io_medium->text(), "serial") == 0)
    {
	snprintf( buf, sizeof(buf), "%s,%s,%d",
		  buf2,
		  serial_port->value(),
		  int(serial_baud->value()));
    }
    else if (strcmp(io_medium->text(), "socket") == 0)
    {
	snprintf( buf, sizeof(buf), "%s,%s,%d,%s",
		  buf2, socket_hostname->value(),
		  int(socket_port->value()),
		  socket_tcp->value() ? "tcp" : "udp" );
    }

    if (io_options_list_value == 0)
    {
	// Add an item to I/O options.
	io_options_list->add( buf );
    }
    else
    {
	// Replace an item in I/O options.
	io_options_list->remove( io_options_list_value );
	io_options_list->insert( io_options_list_value, buf );
    }

    addOptionDlg->hide();
}

void
FGRunUI::io_medium_cb( Fl_Choice* o, void* )
{
    if (strcmp(o->text(), "file") == 0)
    {
	file_group->show();
	// Disable 'bi' menu option for files.
	menu_io_dir[2].deactivate();
	serial_group->hide();
	socket_group->hide();
    }
    else if (strcmp(o->text(), "serial") == 0)
    {
	serial_group->show();
	file_group->hide();
	socket_group->hide();
	menu_io_dir[2].activate();
    }
    else if (strcmp(o->text(), "socket") == 0)
    {
	socket_group->show();
	file_group->hide();
	serial_group->hide();
	menu_io_dir[2].activate();
    }
}

/**
 * Reset all values to their defaults.
 */
void FGRunUI::reset()
{
    fg_root->value( "" );
    fg_scenery->value( "" );
    default_aircraft = "c172";
    default_airport = "KSFO";
    enable_game_mode->value( 0 );
    enable_splash_screen->value( 1 );
    enable_intro_music->value( 1 );
    //enable_mouse_pointer->value(0);
    enable_random_objects->value( 0 );
    freeze->value( 0 );
    fuel_freeze->value( 0 );
    clock_freeze->value( 0 );
    auto_coordination->value( 0 );
//     units_feet->value( 1 );
#if defined(WIN32)
    browser->value( "webrun.bat" );
#else
    browser->value( "netscape" );
#endif
    set_choice( control, "joystick" );
    enable_hud->value( 0 );
    enable_antialias_hud->value( 0 );
    enable_hud->do_callback();
    enable_panel->value( 1 );
    enable_sound->value( 1 );
    enable_clouds->value( 1 );
    enable_clouds3d->value( 0 );
    enable_skyblend->value( 1 );
    enable_textures->value( 1 );
    enable_wireframe->value( 0 );
    enable_fullscreen->value( 0 );
    set_choice( fog, "nice" );
    shading_flat->value( 0 );
    fov->value( 60.0 );
    set_choice( fdm, "jsb" );
    fdm->do_callback();
    notrim->value( 0 );
    on_ground->setonly();
    model_hz->value( 120 );
    speed->value( 1 );
    time_offset_text->value( 0 );
    start_date_sys_text->value( 0 );
    start_date_lat_text->value( 0 );
    start_date_lat_text->value( 0 );
    time_match_real->setonly();
    time_match_real->do_callback();
}
