// settings.cxx -- Save/Load user preferences.
//
// Written by Bernie Bright, started Dec 2002.
//
// Copyright (c) 2002  Bernie Bright - bbright@bigpond.net.au
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

#include <FL/filename.h>
#include "UserInterface.h"

void
UserInterface::save_settings_cb()
{
    Fl_Preferences prefs(Fl_Preferences::USER, "flightgear.org", "fgrun");

    if (fg_exe->value() != 0)
	prefs.set( "fg_exe", fg_exe->value() );
    if (fg_root->value() != 0)
	prefs.set( "fg_root", fg_root->value() );
    if (fg_scenery->value() != 0)
	prefs.set( "fg_scenery", fg_scenery->value() );
    if (aircraft->text() != 0)
	prefs.set( "aircraft", aircraft->text() );
    if (runway->value() != 0)
	prefs.set( "runway", runway->text() );
    if (airport->text() != 0)
	prefs.set( "airport", airport->text() );
    if (browser->value() != 0)
	prefs.set( "browser", browser->value() );
    prefs.set( "control", control->text() );
    if (lang->value() != 0)
	prefs.set("lang", lang->value());
    prefs.set("game_mode", game_mode->value());
    prefs.set("splash_screen", splash_screen->value());
    prefs.set("intro_music", intro_music->value());
    prefs.set("mouse_pointer", mouse_pointer->value());
    prefs.set("random_objects", random_objects->value());
    prefs.set("panel", panel->value());
    prefs.set("sound", sound->value());
    prefs.set("hud", hud->value());
    prefs.set("antialias_hud", antialias_hud->value());
    prefs.set("auto_coordination", auto_coordination->value());

    prefs.set("fdm", fdm->text());
    prefs.set("no_trim", notrim->value());
    prefs.set("model_hz", int(model_hz->value()));
    prefs.set("speed", int(speed->value()));
    prefs.set("on_ground", on_ground->value());
    prefs.set("in_air", in_air->value());
    prefs.set("wind", wind->value());

    prefs.set("freeze", freeze->value());
    prefs.set("fuel_freeze", fuel_freeze->value());
    prefs.set("clock_freeze", clock_freeze->value());

    prefs.set("lon", lon->value());
    prefs.set("lat", lat->value());
    prefs.set("altitude", altitude->value());
    prefs.set("heading", heading->value());
    prefs.set("roll", roll->value());
    prefs.set("pitch", pitch->value());
    prefs.set("vc", vc->value());
    prefs.set("uBody", uBody->value());
    prefs.set("vBody", vBody->value());
    prefs.set("wBody", wBody->value());

    prefs.set("clouds", clouds->value());
    prefs.set("clouds3d", clouds3d->value());
    prefs.set("fullscreen", fullscreen->value());
    prefs.set("skyblend", skyblend->value());
    prefs.set("textures", textures->value());
    prefs.set("wireframe", wireframe->value());
    if (fog_disabled->value())
	prefs.set("fog", "disabled");
    else if (fog_fastest->value())
	prefs.set("fog", "fastest");
    else if (fog_nicest->value())
	prefs.set("fog", "nicest");
    if (shading_smooth->value())
	prefs.set("shading", "smooth");
    else if (shading_flat->value())
	prefs.set("shading", "flat");
    prefs.set("geometry", geometry->value());
    prefs.set("visibility", visibility->value());
    if (vis_meters->value())
	prefs.set("visibility-units", "meters");
    else
	prefs.set("visibility-units", "miles");
    prefs.set("view-offset", view_offset->value());
    prefs.set("bpp", bpp->text());
    prefs.set("fov", fov->value());

    prefs.set("time-match-real", time_match_real->value());
    prefs.set("time-offset", time_offset_value->value());
    prefs.set("time-match-local", time_match_local->value());
    prefs.set("start-date-sys", start_date_sys->value());
    prefs.set("start-date-sys-value", start_date_sys_value->value());
    prefs.set("start-date-gmt", start_date_gmt->value());
    prefs.set("start-date-gmt-value", start_date_gmt_value->value());
    prefs.set("start-date-lat", start_date_lat->value());
    prefs.set("start-date-lat-value", start_date_gmt_value->value());

    if (httpd->value())
	prefs.set("httpd", int(httpd_port->value()));
    if (props->value())
	prefs.set("props", int(props_port->value()));
    if (jpg_httpd->value())
	prefs.set("jpg-httpd", int(jpg_httpd_port->value()));
    prefs.set("network-olk", network_olk->value());
    prefs.set("net-id", net_id->value());
    prefs.set("net-hud", net_hud->value());

    prefs.set("io-count", io_list->size());
    for (int i = 1; i <= io_list->size(); ++i)
	prefs.set( Fl_Preferences::Name("io-item-%d", i), io_list->text(i));
}

void
UserInterface::load_settings_cb()
{
    Fl_Preferences prefs(Fl_Preferences::USER, "flightgear.org", "fgrun");
    const int buflen = 1024;
    char buf[ buflen ];

    prefs.get( "fg_exe", buf, "", buflen-1);
    fg_exe->value(buf);

    // If executable doesn't exist disable "Run" button.
    if (buf[0] != 0) {
	FILE* fp = fopen( buf, "rb" );
	if (fp == 0) {
	    run->deactivate();
	}
	else {
	    fclose(fp);
	}
    }

#if defined(WIN32)
    prefs.get( "fg_root", buf, "\\\\FlightGear", buflen-1);
#else
    prefs.get( "fg_root", buf, "/usr/local/lib/FlightGear", buflen-1);
#endif
    if (fl_filename_isdir(buf))
	fg_root->value(buf);

#if defined(WIN32)
    prefs.get( "fg_scenery", buf,
	       "\\\\FlightGear\\\\Scenery", buflen-1);
#else
    prefs.get( "fg_scenery", buf,
	       "/usr/local/lib/FlightGear/Scenery", buflen-1);
#endif
    if (fl_filename_isdir(buf)) {
	fg_scenery->value(buf);
	airport_update->activate();
	aircraft_update->activate();
    }

    prefs.get("aircraft", buf, "c172", buflen-1);
    default_aircraft = buf;
    prefs.get("airport", buf, "KSFO", buflen-1);
    default_airport = buf;
//     prefs.get( "runway", buf, "<default>", buflen-1 );
//     default_airport = buf;
    prefs.get("lang", buf, "", buflen-1);
    lang->value(buf);
    prefs.get("control", buf, "joystick", buflen-1);
    set_choice( control, buf );

#if defined(WIN32)
    prefs.get("browser", buf, "webrun.bat", buflen-1);
#else
    prefs.get("browser", buf, "netscape", buflen-1);
#endif
    browser->value(buf);

    int iVal;
    double dVal;

    prefs.get("game_mode", iVal, 0);
    game_mode->value(iVal);
    prefs.get("splash_screen", iVal, 1);
    splash_screen->value(iVal);
    prefs.get("intro_music", iVal, 1);
    intro_music->value(iVal);
    prefs.get("mouse_pointer", iVal, 0);
    mouse_pointer->value(iVal);
    prefs.get("random_objects", iVal, 0);
    random_objects->value(iVal);
    prefs.get("panel", iVal, 1);
    panel->value(iVal);
    prefs.get("sound", iVal, 1);
    sound->value(iVal);
    prefs.get("hud", iVal, 0);
    hud->value(iVal);
    hud->do_callback();
    prefs.get("antialias_hud", iVal, 0);
    antialias_hud->value(iVal);
    prefs.get("auto_coordination", iVal, 0);
    auto_coordination->value(iVal);

    prefs.get("fdm", buf, "jsb", buflen-1);
    set_choice(fdm, buf);
    fdm->do_callback();
    prefs.get("notrim", iVal, 0);
    notrim->value(iVal);
    prefs.get("model_hz", iVal, 120);
    model_hz->value(iVal);
    prefs.get("speed", iVal, 1);
    speed->value(iVal);
    prefs.get("on_ground", iVal, 1);
    on_ground->value(iVal);
    prefs.get("in_air", iVal, 0);
    in_air->value(iVal);
    prefs.get("wind", buf, "", buflen-1);
    wind->value(buf);

    prefs.get("freeze", iVal, 0);
    freeze->value(iVal);
    prefs.get("fuel_freeze", iVal, 0);
    fuel_freeze->value(iVal);
    prefs.get("clockfreeze", iVal, 0);
    clock_freeze->value(iVal);

    prefs.get("lon", buf, "", buflen-1);
    lon->value(buf);
    prefs.get("lat", buf, "", buflen-1);
    lat->value(buf);
    prefs.get("altitude", buf, "", buflen-1);
    altitude->value(buf);
    prefs.get("heading", dVal, 0.0);
    heading->value(dVal);
    prefs.get("roll", dVal, 0.0);
    roll->value(dVal);
    prefs.get("pitch", dVal, 0.0);
    pitch->value(dVal);
    prefs.get("vc", buf, "", buflen-1);
    vc->value(buf);
    prefs.get("uBody", buf, "", buflen-1);
    uBody->value(buf);
    prefs.get("vBody", buf, "", buflen-1);
    vBody->value(buf);
    prefs.get("wBody", buf, "", buflen-1);
    wBody->value(buf);

    prefs.get("clouds", iVal, 1);
    clouds->value(iVal);
    prefs.get("clouds3d", iVal, 0);
    clouds3d->value(iVal);
    prefs.get("fullscreen", iVal, 0);
    fullscreen->value(iVal);
    prefs.get("skyblend", iVal, 1);
    skyblend->value(iVal);
    prefs.get("textures", iVal, 1);
    textures->value(iVal);
    prefs.get("wireframe", iVal, 0);
    wireframe->value(iVal);
    prefs.get("shading", buf, "smooth", buflen-1);
    if (strcmp(buf, "smooth") == 0)
	shading_smooth->setonly();
    else if (strcmp(buf, "flat") == 0)
	shading_flat->setonly();
    prefs.get("fog", buf, "nicest", buflen-1);
    if (strcmp(buf, "nicest") == 0)
	fog_nicest->setonly();
    else if (strcmp(buf, "fastest") == 0)
	fog_fastest->setonly();
    else if (strcmp(buf, "disabled") == 0)
	fog_disabled->setonly();
    prefs.get("geometry", buf, "", buflen-1);
    geometry->value(buf);
    prefs.get("visibility", buf, "", buflen-1);
    visibility->value(buf);
    prefs.get("view-offset", buf, "", buflen-1);
    view_offset->value(buf);
    prefs.get("bpp", buf, "16", buflen-1);
    set_choice( bpp, buf );
    prefs.get("fov", dVal, 60.0 );
    fov->value( dVal );
    prefs.get("visibility-units", buf, "meters", buflen-1);
    if (strcmp(buf,"meters") == 0)
	vis_meters->setonly();
    else if (strcmp(buf,"miles") == 0)
	vis_miles->setonly();

    prefs.get( "time-match-real", iVal, 1 );
    if (iVal) time_match_real->setonly();
    prefs.get( "time-offset", buf, "", buflen-1 );
    time_offset_value->value( buf );
    prefs.get( "time-match-local", iVal, 0 );
    if (iVal) time_match_local->setonly();
    prefs.get( "start-date-sys", iVal, 0 );
    if (iVal) start_date_sys->setonly();
    prefs.get( "start-date-sys-value", buf, "", buflen-1 );
    start_date_sys_value->value(buf);
    prefs.get( "start-date-gmt", iVal, 0 );
    if (iVal) start_date_gmt->setonly();
    prefs.get( "start-date-gmt-value", buf, "", buflen-1 );
    start_date_gmt_value->value(buf);
    prefs.get( "start-date-lat", iVal, 0 );
    if (iVal) start_date_lat->setonly();
    prefs.get( "start-date-lat-value", buf, "", buflen-1 );
    start_date_lat_value->value(buf);

    prefs.get( "io-count", iVal, 0 );
    for (int i = 1; i <= iVal; ++i)
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("io-item-%d", i), buf, "", buflen-1 );
	io_list->add( "" );
	io_list->text( i, buf );
    }
}

void
UserInterface::default_settings_cb()
{
}
