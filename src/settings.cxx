// settings.cxx -- Functions to load and save settings.
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

#include <FL/Fl_Preferences.h>
#include <FL/filename.h>
#include "FGRunUI.h"

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

    if (fg_exe->value() != 0)
	prefs.set( "fg_exe", fg_exe->value() );
    if (fg_root->value() != 0)
	prefs.set( "fg_root", fg_root->value() );
    if (fg_scenery->value() != 0)
	prefs.set( "fg_scenery", fg_scenery->value() );
    if (aircraft->text() != 0)
	prefs.set( "aircraft", aircraft->text() );
    if (airport->text() != 0)
	prefs.set( "airport", airport->text() );
    if (browser->value() != 0)
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

