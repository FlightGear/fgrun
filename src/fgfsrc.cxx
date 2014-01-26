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
#include <string.h>        // strcmp
#endif

#include "wizard.h"
#include "i18n.h"

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
        int r = fl_choice( _("About to overwrite %s."), _("Abort"), _("Overwrite"), 0, buf );
        if (!r)
            return 0;
    }

    ofstream ofs( buf );
    if (ofs)
    {
        int rc = write_fgfsrc( prefs, ofs );
        ofs << "\n";
        ofs.close();
        return rc;
    }

    return 0;
}

int
Wizard::write_fgfsrc( Fl_Preferences &prefs, std::ostream& os, const char* pfx )
{
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

    prefs.get( "fg_root", buf, "", buflen-1 );
    os << "--fg-root=" << buf;

    prefs.get( "fg_scenery", buf, "", buflen-1 );
    os << pfx << "--fg-scenery=" << buf;

    if (prefs.get( "ts_dir", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--terrasync-dir=" << buf;

    prefs.get( "fg_aircraft", buf, "", buflen-1 );
    if ( buf[0] != 0 )
        os << pfx << "--fg-aircraft=" << buf;

    // General options.
    if (prefs.get( "carrier", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--carrier=" << buf;
    else if (prefs.get( "airport", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--airport=" << buf;

    if (prefs.get( "runway", buf, "", buflen-1 ) &&
        strcmp( _("<default>"), buf ) != 0)
        os << pfx << "--runway=" << buf;

    if (prefs.get( "aircraft", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--aircraft=" << buf;

    if (prefs.get( "parkpos", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--parkpos=" << buf;

    if (prefs.get( "control", buf, "", buflen-1 ))
        os << pfx << "--control=" << buf;
    if (prefs.get( "lang", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--language=" << buf;
    if (prefs.get( "browser", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--browser-app=" << buf;
    if (prefs.get( "config", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--config=" << buf;
  
    int iVal, iVal2;
    double dVal;

    // Features - only set non-default values.
    if (prefs.get( "splash_screen", iVal, 1 ) && !iVal)
        os << pfx << "--disable-splash-screen";
    if (prefs.get( "mouse_pointer", iVal, 0 ) && iVal)
        os << pfx << "--enable-mouse-pointer";
    if (prefs.get( "random_objects", iVal, 0 ) && iVal)
        os << pfx << "--enable-random-objects";
    else
        os << pfx << "--disable-random-objects";
    if (prefs.get( "random_trees", iVal, 1 ) && !iVal)
        os << pfx << "--prop:/sim/rendering/random-vegetation=false";

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
        if (prefs.get( "failure_electrical", iVal, 0 ) && iVal)
            os << pfx << "--failure=electrical";
        if (prefs.get( "failure_vacuum", iVal, 0 ) && iVal)
            os << pfx << "--failure=vacuum";
    }

    prefs.get( "ai_models", iVal, 0 );
    if (iVal)
        os << pfx << "--enable-ai-models";
    else
        os << pfx << "--disable-ai-models";

    prefs.get( "ai_traffic", iVal, 0 );
    if (iVal)
        os << pfx << "--enable-ai-traffic";
    else
        os << pfx << "--disable-ai-traffic";

    // Flight model
    prefs.get( "fdm", buf, "", buflen-1 );
    if (buf[0] != 0)
    {
        if (strcmp( "automatic selection", buf ) != 0)
            os << pfx << "--fdm=" << buf;
        else if (prefs.get( "no_trim", iVal, 0 ) && iVal)
            os << pfx << "--notrim"; // Only "jsb" understands --notrim
    }

    if (prefs.get( "model_hz", iVal, 0 ) && iVal != 120)
        os << pfx << "--model-hz=" << iVal;
    if (prefs.get( "speed", iVal, 0 ) && iVal > 1)
        os << pfx << "--speed=" << iVal;
    if (prefs.get( "in_air", iVal, 0 ) && iVal)
        os << pfx << "--in-air";

    // Weather
    if (prefs.get( "random_wind", iVal, 0 ) && iVal)
    {
        os << pfx << "--random-wind";
    }
    else if (prefs.get( "wind_speed", dVal, 0.0 ) && dVal > 0.0)
    {
        double dVal2;
        prefs.get( "wind_heading", dVal2, 0.0 );
        os << pfx << "--wind=" << dVal2 << "@" << dVal;
    }

    if (prefs.get( "turbulence", dVal, 0. ) && dVal > 0.)
        os << pfx << "--turbulence=" << dVal;
    if (prefs.get( "ceiling", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--ceiling=" << buf;
    if (prefs.get( "fetch_real_weather", iVal, 0 ) && iVal)
        os << pfx << "--enable-real-weather-fetch";
    else
        os << pfx << "--disable-real-weather-fetch";

    // Freeze
    if (prefs.get( "freeze", iVal, 0 ) && iVal)
        os << pfx << "--enable-freeze";
    if (prefs.get( "fuel_freeze", iVal, 0 ) && iVal)
        os << pfx << "--enable-fuel-freeze";
    if (prefs.get( "clock_freeze", iVal, 0 ) && iVal)
        os << pfx << "--enable-clock-freeze";
        
    // Initial position and orientation.
    if (prefs.get( "lon", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--lon=" << buf;
    if (prefs.get( "lat", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--lat=" << buf;
    if (prefs.get( "altitude", buf, "", buflen-1 ) && buf[0] != 0)
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
    if (prefs.get( "vor", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--vor=" << buf;
    if (prefs.get( "ndb", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--ndb=" << buf;
    if (prefs.get( "fix", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--fix=" << buf;
    if (prefs.get( "offset_distance", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--offset-distance=" << buf;
    if (prefs.get( "offset_azimuth", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--offset-azimuth=" << buf;
    if (prefs.get( "glideslope", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--glideslope=" << buf;

    // Rendering.
    if (prefs.get( "clouds", iVal, 1 ) && !iVal)
        os << pfx << "--disable-clouds";
    if (prefs.get( "clouds3d", iVal, 0 ) && iVal)
        os << pfx << "--enable-clouds3d";
    if (prefs.get( "fullscreen", iVal, 0 ) && iVal)
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
    if (prefs.get( "frame_rate_limiter", iVal, 0 ) && iVal &&
             prefs.get( "frame_rate_limiter_value", iVal2, 60 ))
        os << pfx << "--prop:/sim/frame-rate-throttle-hz=" << iVal2;

    if (prefs.get( "autovisibility", iVal, 0 ) && iVal)
        os << pfx << "--prop:/sim/menubar/autovisibility/enabled=1";

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

    if (prefs.get( "view-offset", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--view-offset=" << buf;
    if (prefs.get( "bpp", iVal, 16) && iVal != 16)
        os << pfx << "--bpp=" << iVal;
    if (prefs.get( "fov", dVal, 60.0 ) && dVal != 60.)
        os << pfx << "--fov=" << dVal;
    if (prefs.get( "texture-filtering", buf, "1", buflen-1 ) && strcmp(buf,"1")!=0)
        os << pfx << "--texture-filtering=" << buf;
    if (prefs.get( "materials-file", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--materials-file=" << buf;
    if (prefs.get( "rembrandt", iVal, 0 ) && iVal)
        os << pfx << "--enable-rembrandt";
    else if (prefs.get( "anti-aliasing", buf, "1", buflen-1 ) && strcmp(buf,"1")!=0) {
        os << pfx << "--prop:/sim/rendering/multi-sample-buffers=1";
        os << pfx << "--prop:/sim/rendering/multi-samples=" << buf;
    }

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

    if ( prefs.get( "season", buf, "", buflen-1 ) && strcmp(buf,"summer")!=0 )
        os << pfx << "--season=" << buf;

    if ( prefs.get( "terrasync", iVal, 1 ) && iVal )
        os << pfx << "--enable-terrasync";
    else
        os << pfx << "--disable-terrasync";

    // Network.
    if (prefs.get( "httpd", iVal, 0 ) && iVal)
        os << pfx << "--httpd=" << iVal;
    if (prefs.get( "props", iVal, 0 ) && iVal)
        os << pfx << "--props=" << iVal;
    if (prefs.get( "jpg-httpd", iVal, 0 ) && iVal)
        os << pfx << "--jpg-httpd=" << iVal;

    // Multiplayer options.
    if (prefs.get( "callsign", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--callsign=" << buf;
    if (prefs.get( "multiplay1", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--multiplay=" << buf;
    if (prefs.get( "multiplay2", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--multiplay=" << buf;
    // FGCom options.
    if ( prefs.get( "fgcom-disabled", iVal, 1 ) && iVal )
        os << pfx << "--disable-fgcom";
    if ( prefs.get( "fgcom-builtin", iVal, 1 ) && iVal )
        os << pfx << "--enable-fgcom";
    if ( prefs.get( "fgcom-standalone", iVal, 1 ) && iVal ) {
         char buf1[ buflen ];
         if ( prefs.get( "fgcom-hostname", buf, "", buflen-1 ) &&
              prefs.get( "fgcom-port", buf1, "", buflen-1 ) &&
              buf[0] != 0 && buf1[0] != 0)
              os << pfx << "--generic=socket,out,2," << buf << "," << buf1 << ",udp,fgcom";
    }

    if (prefs.get( "proxy", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--proxy=" << buf;

    // Scenarii options.
    prefs.get( "scenario-count", iVal, 0 );
    int i;
    for (i = 1; i <= iVal; ++i)
    {
        buf[0] = 0;
        prefs.get( Fl_Preferences::Name("scenario-item-%d", i),
                   buf, "", buflen-1 );
        if ( strlen( buf ) > 0 )
            os << pfx << "--ai-scenario=" << buf;
    }

    // I/O options.
    prefs.get( "io-count", iVal, 0 );
    for (i = 1; i <= iVal; ++i)
    {
        buf[0] = 0;
        prefs.get( Fl_Preferences::Name("io-item-%d", i),
                   buf, "", buflen-1 );
        if ( strlen( buf ) > 0 )
            os << pfx << buf;
    }

    // Properties
    prefs.get( "property-count", iVal, 0 );
    for (i = 1; i <= iVal; ++i)
    {
        buf[0] = 0;
        prefs.get( Fl_Preferences::Name("property-item-%d", i),
                   buf, "", buflen-1 );
        if ( strlen( buf ) > 0 )
        {
#if defined(WIN32)
            if (strchr(buf,' ') != 0)
                os << pfx << "\"--prop:" << buf << "\"";
            else
#endif
                os << pfx << "--prop:" << buf;
        }
    }

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
    if (prefs.get( "nav1", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--nav1=" << buf;
    if (prefs.get( "nav2", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--nav2=" << buf;
    if (prefs.get( "adf", buf, "", buflen-1 ) && buf[0] != 0)
        os << pfx << "--adf=" << buf;
    prefs.get( "dme", buf, "disabled", buflen-1 );
    if (strcmp( "disabled", buf ) != 0)
        os << pfx << "--dme=" << buf;

    // Clouds
    for (i = 0; i < 5; ++i)
    {
        double el, th;

        prefs.get( Fl_Preferences::Name("layer-%d-elevation-ft", i), el, 0.0 );
        prefs.get( Fl_Preferences::Name("layer-%d-thickness-ft", i), th, 0.0 );
        if (el > 0.0 && th > 0.0 )
        {
            os << pfx << "--prop:/environment/clouds/layer[" << i
               << "]/elevation-ft=" << el;

            os << pfx << "--prop:/environment/clouds/layer[" << i
               << "]/thickness-ft=" << th;

            prefs.get( Fl_Preferences::Name("layer-%d-coverage", i),
                       buf, "clear", buflen-1);
            os << pfx << "--prop:/environment/clouds/layer[" << i
               << "]/coverage=" << buf;

            prefs.get( Fl_Preferences::Name("layer-%d-transition-ft", i),
                       dVal, 0.0 );
            if (dVal > 0.0)
                os << pfx << "--prop:/environment/clouds/layer[" << i
                   << "]/transition-ft=" << dVal;

            prefs.get( Fl_Preferences::Name("layer-%d-span-m", i),
                       dVal, 0.0 );
            if (dVal > 0.0)
                os << pfx << "--prop:/environment/clouds/layer[" << i
                   << "]/span-m=" << dVal;
        }
    }

    return 1;
}

int
Wizard::write_fgviewerrc( Fl_Preferences &prefs, std::ostream& os, const char* pfx, const std::string &path )
{
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

    prefs.get( "fg_root", buf, "", buflen-1 );
    os << "--fg-root=" << buf;

    prefs.get( "fg_aircraft", buf, "", buflen-1 );
    if ( buf[0] != 0 )
        os << pfx << "--fg-aircraft=" << buf;

    os << pfx << "--fgviewer";
    os << pfx << path;

    return 1;
}
