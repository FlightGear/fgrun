// fgfsrc.cxx -- Function to write FlightGear startup file.
//
// Written by Bernie Bright, started Dec 2002.
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

#include <cstdio>
#include <fstream>

#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Preferences.H>

#if defined(HAVE_STRING_H)
#include <string.h>	// strcmp
#endif

#include "wizard.h"

using std::ofstream;

int
Wizard::write_fgfsrc()
{
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

#if defined(WIN32)
    prefs.get( "fg_root", buf, "", buflen-1 );
    strcat( buf, "/system.fgfsrc" );
    fl_filename_absolute( buf, buf );
#else
    fl_filename_expand( buf, "~/.fgfsrc" );
#endif

    FILE* fp = fopen( buf, "r" );
    if (fp != 0)
    {
	fclose( fp );
	int r = fl_ask( "About to overwrite %s.\nDo you want to continue?",
			buf );
	if (!r)
	    return 0;
    }

    ofstream ofs( buf );
    if (ofs)
    {
	int rc = write_fgfsrc( ofs );
	ofs << "\n";
	ofs.close();
	return rc;
    }

    return 0;
}

int
Wizard::write_fgfsrc( std::ostream& os, const char* pfx )
{
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

    prefs.get( "fg_root", buf, "", buflen-1 );
    os << "--fg-root=" << buf;

    prefs.get( "fg_scenery", buf, "", buflen-1 );
    os << pfx << "--fg-scenery=" << buf;

    // General options.
    if (prefs.get( "airport", buf, "", buflen-1 ))
	os << pfx << "--airport-id=" << buf;

    if (prefs.get( "runway", buf, "", buflen-1 ) &&
	strcmp( "<default>", buf ) != 0)
	os << pfx << "--runway=" << buf;

    if (prefs.get( "aircraft", buf, "", buflen-1 ))
	os << pfx << "--aircraft=" << buf;

    if (prefs.get( "control", buf, "", buflen-1 ))
	os << pfx << "--control=" << buf;
    if (prefs.get( "lang", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--lang=" << buf;
    if (prefs.get( "browser", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--browser-app=" << buf;
  
    int iVal;
    double dVal;

    // Features - only set non-default values.
    if (prefs.get( "game_mode", iVal, 0 ) && iVal)
	os << pfx << "--enable-game-mode";
    if (prefs.get( "splash_screen", iVal, 1 ) && !iVal)
	os << pfx << "--disable-splash-screen";
    if (prefs.get( "intro_music", iVal, 1 ) && !iVal)
	os << pfx << "--disable-intro-music";
// 	if (prefs.get( "mouse_pointer", iVal, 0 ) && iVal)
// 	    os << pfx << "--enable-mouse-pointer";
    if (prefs.get( "random_objects", iVal, 0 ) && iVal)
	os << pfx << "--enable-random-objects";
    else
	os << pfx << "--disable-random-objects";

    if (prefs.get( "panel", iVal, 1 ) && !iVal)
	os << pfx << "--disable-panel";
    if (prefs.get( "sound", iVal, 1 ) && !iVal)
	os << pfx << "--disable-sound";
    if (prefs.get( "hud", iVal, 0 ) && iVal)
    {
	os << pfx << "--enable-hud";
	if (prefs.get( "antialias_hud", iVal, 0 ) && iVal)
	    os << pfx << "--enable-anti-alias-hud";
	else
	    os << pfx << "--disable-anti-alias-hud";
    }

    if (prefs.get( "hud_3d", iVal, 1 ) && !iVal)
	os << pfx << "--disable-hud-3d";
    if (prefs.get( "auto_coordination", iVal, 0 ) && iVal)
	os << pfx << "--enable-auto-coordination";
    if (prefs.get( "horizon_effect", iVal, 0 ) && iVal)
	os << pfx << "--enable-horizon-effect";
    if (prefs.get( "enhanced_lighting", iVal, 0 ) && iVal)
	os << pfx << "--enable-enhanced-lighting";
    if (prefs.get( "distance_attenuation", iVal, 0 ) && iVal)
	os << pfx << "--enable-distance-attenuation";
    if (prefs.get( "specular_highlight", iVal, 1 ) && !iVal)
	os << pfx << "--disable-specular-highlight";

    if (prefs.get( "failure", iVal, 0 ) && iVal)
    {
	if (prefs.get( "failure_pitot", iVal, 0 ) && iVal)
	    os << pfx << "--failure=pitot";
	if (prefs.get( "failure_static", iVal, 0 ) && iVal)
	    os << pfx << "--failure=static";
	if (prefs.get( "failure_system", iVal, 0 ) && iVal)
	    os << pfx << "--failure=system";
	if (prefs.get( "failure_vacuum", iVal, 0 ) && iVal)
	    os << pfx << "--failure=vacuum";
    }

    // Flight model
    prefs.get( "fdm", buf, "", buflen-1 );
    if (buf[0] != 0)
    {
	if (strcmp( "jsb", buf ) != 0)
	    os << pfx << "--fdm=" << buf;
	else if (prefs.get( "no_trim", iVal, 0 ) && iVal)
	    os << pfx << "--notrim"; // Only "jsb" understands --notrim
    }

    if (prefs.get( "model_hz", iVal, 0 ) && iVal != 120)
	os << pfx << "--model_hz=" << iVal;
// 	if (speed->value() != 1)
// 	    os << pfx << "--speed=" << speed->value();
    if (prefs.get( "in_air", iVal, 0 ) && iVal)
	os << pfx << "--in-air";
    if (prefs.get( "wind", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--wind=" << buf;
    if (prefs.get( "turbulence", dVal, 0. ) && dVal > 0.)
	os << pfx << "--turbulence=" << dVal;
    if (prefs.get( "ceiling", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--ceiling=" << buf;

    // Freeze
    if (prefs.get( "freeze", iVal, 0 ) && iVal)
	os << pfx << "--enable-freeze";
    if (prefs.get( "fuel_freeze", iVal, 0 ) && iVal)
	os << pfx << "--enable-fuel-freeze";
    if (prefs.get( "clockfreeze", iVal, 0 ) && iVal)
	os << pfx << "--enable-clock-freeze";
	
    // Initial position and orientation.
    if (prefs.get( "lon", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--lon=" << buf;
    if (prefs.get( "lat", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--lat=" << buf;
    if (prefs.get( "lat", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--altitude=" << buf;
    if (prefs.get( "heading", dVal, 0. ) && dVal != 0.)
	os << pfx << "--heading=" << dVal;
    if (prefs.get( "roll", dVal, 0. ) && dVal != 0.)
	os << pfx << "--roll=" << dVal;
    if (prefs.get( "pitch", dVal, 0. ) && dVal != 0.)
	os << pfx << "--pitch=" << dVal;
    if (prefs.get( "vc", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--vc=" << buf;
    if (prefs.get( "uBody", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--uBody=" << buf;
    if (prefs.get( "vBody", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--vBody=" << buf;
    if (prefs.get( "wBody", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--wBody=" << buf;

    // Rendering.
    if (prefs.get( "clouds", iVal, 1 ) && !iVal)
	os << pfx << "--disable-clouds";
    if (prefs.get( "clouds3d", iVal, 0 ) && iVal)
	os << pfx << "--enable-clouds3d";
    if (prefs.get( "fullsceen", iVal, 0 ) && iVal)
	os << pfx << "--enable-fullscreen";
    if (prefs.get( "skyblend", iVal, 1 ) && !iVal)
	os << pfx << "--disable-skyblend";
    if (prefs.get( "textures", iVal, 1 ) && !iVal)
	os << pfx << "--disable-textures";
    if (prefs.get( "wireframe", iVal, 0 ) && iVal)
	os << pfx << "--enable-wireframe";
    if (prefs.get( "shading", buf, "", buflen-1 ) &&
	strcmp( "flat", buf ) == 0)
	os << pfx << "--shading-flat";

    if (prefs.get( "fog", buf, "", buflen-1 ))
    {
	if (strcmp( "disabled", buf ) == 0)
	    os << pfx << "--fog-disable";
	else if (strcmp( "fastest", buf ) == 0)
	    os << pfx << "--fog-fastest";
    }

    if (prefs.get( "geometry", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--geometry=" << buf;

    if (prefs.get( "visibility", buf, "", buflen-1 ) && buf[0] != 0)
    {
	char buf2[ buflen ];
	prefs.get( "visibility-units", buf2, "", buflen-1 );
	if (strcmp( "meters", buf2 ) == 0)
	    os << pfx << "--visibility=" << buf;
	else if (strcmp( "miles", buf2 ) == 0)
	    os << pfx << "--visibility-miles=" << buf;
    }

    // ?view-offset?
    if (prefs.get( "bpp", iVal, 16) && iVal != 16)
	os << pfx << "--bpp=" << iVal;
    if (prefs.get( "fov", dVal, 60.0 ) && dVal != 60.)
	os << pfx << "--fov=" << dVal;

    // Time
    if (prefs.get( "time-match-real", iVal, 1 ) && iVal &&
	prefs.get( "time-offset", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--time-offset=" << buf;
    else if (prefs.get( "time-match-local", iVal, 0 ) && iVal)
	os << pfx << "--time-match-local";
    else if (prefs.get( "start-date-sys", iVal, 0 ) && iVal &&
	     prefs.get( "start-date-sys-value", buf, "", buflen-1 ) &&
	     buf[0] != 0)
	os << pfx << "--start-date-sys=" << buf;
    else if (prefs.get( "start-date-gmt", iVal, 0 ) && iVal &&
	     prefs.get( "start-date-gmt-value", buf, "", buflen-1 ) &&
	     buf[0] != 0)
	os << pfx << "--start-date-gmt=" << buf;
    else if (prefs.get( "start-date-lat", iVal, 0 ) && iVal &&
	     prefs.get( "start-date-lat-value", buf, "", buflen-1 ) &&
	     buf[0] != 0)
	os << pfx << "--start-date-lat=" << buf;
    else if (prefs.get( "time_of_day", iVal, 0 ) && iVal &&
	     prefs.get( "time_of_day_value", buf, "", buflen-1 ) &&
	     buf[0] != 0)
	os << pfx << "--timeofday=" << buf;

    // Network.
    if (prefs.get( "httpd", iVal, 0 ) && iVal)
	os << pfx << "--httpd=" << iVal;
    if (prefs.get( "props", iVal, 0 ) && iVal)
	os << pfx << "--props=" << iVal;
    if (prefs.get( "jpg-httpd", iVal, 0 ) && iVal)
	os << pfx << "--jpg_httpd=" << iVal;
    if (prefs.get( "callsign", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--callsign=" << buf;
    if (prefs.get( "multiplay", buf, "", buflen-1 ) && buf[0] != 0)
	os << pfx << "--multiplay=" << buf;

    // I/O options.
    prefs.get( "io-count", iVal, 0 );
    int i;
    for (i = 1; i <= iVal; ++i)
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("io-item-%d", i),
		   buf, "", buflen-1 );
	os << pfx << "" << buf;
    }

    // Properties
// 	for (i = 1; i <= prop_list->size(); ++i)
// 	{
// 	    os << pfx << "--prop:" << prop_list->text(i);
// 	}

    // Debugging
    prefs.get( "log-level", buf, "", buflen-1 );
    if (buf[0] != 0 && strcmp( "alert", buf ) != 0)
	os << pfx << "--log-level=" << buf;

    prefs.get( "trace-read-count", iVal, 0 );
    for (i = 1; i <= iVal; ++i)
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("trace-read-%d", i),
		   buf, "", buflen-1 );
	os << pfx << "--trace-read=" << buf;
    }

    prefs.get( "trace-write-count", iVal, 0 );
    for (i = 1; i <= iVal; ++i)
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("trace-write-%d", i),
		   buf, "", buflen-1 );
	os << pfx << "--trace-write=" << buf;
    }

    // Avionics
// 	if (nav1->size() > 1)
// 	    os << pfx << "--nav1=" << nav1->value();
// 	if (nav2->size() > 1)
// 	    os << pfx << "--nav2=" << nav2->value();
// 	if (adf->size() > 1)
// 	    os << pfx << "--adf=" << adf->value();
// 	if (dme->value())
// 	{
// 	    if (dme_nav1->value())
// 		os << pfx << "--dme=nav1";
// 	    else if (dme_nav2->value())
// 		os << pfx << "--dme=nav2";
// 	    else if (dme_int->value())
// 		os << pfx << "--dme=" << dme_int_freq->value();
// 	}

    return 1;
}
