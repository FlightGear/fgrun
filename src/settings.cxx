// settings.cxx -- Save/Load user preferences.
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
#  include "config.h"
#endif

#include <string>

#include <FL/filename.h>

#include "advanced.h"
#include "util.h"

using std::string;

static const char*
coverage_to_string( int coverage )
{
    switch (coverage)
    {
    case 0: return "overcast";
    case 1: return "broken";
    case 2: return "scattered";
    case 3: return "few";
    case 4: return "cirrus";
    case 5: return "clear";
    default: return "clear";
    };
}

static int
string_to_coverage( const char* s )
{
    if (strcmp( "overcast", s ) == 0)
	return 0;
    else if (strcmp( "broken", s ) == 0)
	return 1;
    else if (strcmp( "scattered", s ) == 0)
	return 2;
    else if (strcmp( "few", s ) == 0)
	return 3;
    else if (strcmp( "cirrus", s ) == 0)
	return 4;
    else if (strcmp( "clear", s ) == 0)
	return 5;
    else
	return 5;
}

void
Advanced::save_settings( Fl_Preferences& prefs )
{
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

    prefs.set( "browser", browser->value() );
    prefs.set( "control", control->text() );
    prefs.set( "lang", lang->value());
    prefs.set( "config", config->value() );

    prefs.set("game_mode", game_mode->value());
    prefs.set("splash_screen", splash_screen->value());
    prefs.set("intro_music", intro_music->value());
    prefs.set("mouse_pointer", mouse_pointer->value());
    prefs.set("random_objects", random_objects->value());
    prefs.set("panel", panel->value());
    prefs.set("sound", sound->value());
    prefs.set("hud", hud->value());
    prefs.set("antialias_hud", antialias_hud->value());
    prefs.set("hud_3d", hud_3d->value());
    prefs.set("auto_coordination", auto_coordination->value());
    prefs.set("horizon_effect", horizon_effect->value());
    prefs.set("enhanced_lighting", enhanced_lighting->value());
    prefs.set("distance_attenuation", distance_attenuation->value());
    prefs.set("specular_highlight", specular_highlight->value());
    prefs.set("failure", failure->value());
    prefs.set("failure_pitot", failure_pitot->value());
    prefs.set("failure_static", failure_static->value());
    prefs.set("failure_system", failure_system->value());
    prefs.set("failure_vacuum", failure_vacuum->value());

    prefs.set("fdm", fdm->text());
    prefs.set("no_trim", notrim->value());
    prefs.set("model_hz", int(model_hz->value()));
    prefs.set("speed", int(speed->value()));
    prefs.set("on_ground", on_ground->value());
    prefs.set("in_air", in_air->value());
    prefs.set("wind_heading", wind_dial->value());
    prefs.set("wind_speed", wind_speed->value());
    prefs.set("turbulence", turbulence->value());
    prefs.set("ceiling", ceiling->value());

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
    prefs.set("time_of_day", time_of_day->value());
    prefs.set("time_of_day_value", time_of_day_value->text());

    // Network options.
    if (httpd->value())
	prefs.set("httpd", int(httpd_port->value()));
    if (props->value())
	prefs.set("props", int(props_port->value()));
    if (jpg_httpd->value())
	prefs.set("jpg-httpd", int(jpg_httpd_port->value()));
    // Multiplayer options
    prefs.set( "callsign", callsign->value() );
    prefs.set( "multiplay1", multiplay1->value() );
    prefs.set( "multiplay2", multiplay2->value() );

    prefs.set("io-count", io_list->size());
    int i;
    for (i = 1; i <= io_list->size(); ++i)
	prefs.set( Fl_Preferences::Name("io-item-%d", i), io_list->text(i));

    prefs.set("property-count", prop_list->size());
    for (i = 1; i <= prop_list->size(); ++i)
	prefs.set( Fl_Preferences::Name("property-item-%d", i),
		   prop_list->text(i));

    prefs.set("env-count", env_list->size());
    for (i = 1; i <= env_list->size(); ++i)
	prefs.set( Fl_Preferences::Name("env-var-%d", i), env_list->text(i));

    prefs.set( "log-level", log_level->text() );
    prefs.set("trace-read-count", trace_read_list->size());
    for (i = 1; i <= trace_read_list->size(); ++i)
	prefs.set( Fl_Preferences::Name("trace-read-%d", i),
		   trace_read_list->text(i) );
    prefs.set("trace-write-count", trace_write_list->size());
    for (i = 1; i <= trace_write_list->size(); ++i)
	prefs.set( Fl_Preferences::Name("trace-write-%d", i),
		   trace_write_list->text(i) );

    prefs.set( "nav1", nav1->value() );
    prefs.set( "nav2", nav2->value() );
    prefs.set( "adf", adf->value() );

    if (dme->value())
    {
	if (dme_nav1->value())
	    prefs.set( "dme", "nav1" );
	else if (dme_nav2->value())
	    prefs.set( "dme", "nav2" );
	else if (dme_int->value())
	    prefs.set( "dme", dme_int_freq->value() );
    }
    else
    {
	prefs.set( "dme", "disabled" );
    }

    for( i = 0; i < MAX_CLOUD_LAYERS; ++i)
    {
	prefs.set( Fl_Preferences::Name("layer-%d-elevation-ft", i),
		   cloud_elevation[i] );
	prefs.set( Fl_Preferences::Name("layer-%d-thickness-ft", i),
		   cloud_thickness[i] );
	prefs.set( Fl_Preferences::Name("layer-%d-coverage", i),
		   coverage_to_string( cloud_coverage[i] ) );
	prefs.set( Fl_Preferences::Name("layer-%d-transition-ft", i),
		   cloud_transition[i] );
	prefs.set( Fl_Preferences::Name("layer-%d-span-m", i),
		   cloud_span[i] );
    }
}

void
Advanced::load_settings( Fl_Preferences& prefs )
{
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];
    const char* not_set = "NOT SET";
    int i = 0;

    prefs.get( "fg_exe", buf, not_set, buflen-1);
    fg_exe_->value(buf);

    prefs.get( "fg_root", buf, not_set, buflen-1);
    fg_root_->value( buf );

    prefs.get( "fg_scenery", buf, not_set, buflen-1 );
    fg_scenery_->value( buf );

    prefs.get( "aircraft", buf, not_set, buflen-1 );
    aircraft_->value( buf );

    prefs.get( "airport", buf, "", buflen-1 );
    string s( buf );
    prefs.get( "airport-name", buf, "", buflen-1);
    if (buf[0] != 0)
    {
	s.append( " - " );
	s.append( buf );
    }
    airport_->value( s.c_str() );

    prefs.get( "runway", buf, not_set, buflen-1 );
    runway_->value( buf );

    prefs.get( "lang", buf, "", buflen-1 );
    lang->value( buf );

    prefs.get( "control", buf, "joystick", buflen-1 );
    set_choice( control, buf );

// #if defined(WIN32)
//     prefs.get("browser", buf, "webrun.bat", buflen-1);
// #else
//     prefs.get("browser", buf, "netscape", buflen-1);
// #endif
//     browser->value(buf);

    if (prefs.get( "config", buf, "", buflen-1) && buf[0] != 0)
	config->value( buf );

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
    prefs.get("hud_3d", iVal, 1);
    hud_3d->value(iVal);
    prefs.get("auto_coordination", iVal, 0);
    auto_coordination->value(iVal);
    prefs.get("horizon_effect", iVal, 0);
    horizon_effect->value( iVal );
    prefs.get("enhanced_lighting", iVal, 0);
    enhanced_lighting->value( iVal );
    prefs.get("distance_attenuation", iVal, 0);
    distance_attenuation->value( iVal );
    prefs.get("specular_highlight", iVal, 1);
    specular_highlight->value( iVal );
    prefs.get("failure", iVal, 0);
    failure->value( iVal );
    failure->do_callback();
    prefs.get("failure_pitot", iVal, 0);
    failure_pitot->value( iVal );
    prefs.get("failure_static", iVal, 0);
    failure_static->value( iVal );
    prefs.get("failure_system", iVal, 0);
    failure_system->value( iVal );
    prefs.get("failure_vacuum", iVal, 0);
    failure_vacuum->value( iVal );

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
    prefs.get( "wind_heading", dVal, 0.0 );
    wind_dial->value( dVal );
    wind_hdg->value( dVal );
    prefs.get( "wind_speed", dVal, 0.0 );
    wind_speed->value( dVal );
    prefs.get("turbulence", dVal, 0.0 );
    turbulence->value(dVal);
    prefs.get("ceiling", buf, "", buflen-1);
    ceiling->value(buf);

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
    prefs.get( "time_of_day", iVal, 0 );
    if (iVal) time_of_day->setonly();
    prefs.get( "time_of_day_value", buf, "dawn", buflen-1 );
    set_choice( time_of_day_value, buf );

    prefs.get( "httpd", iVal, 0 );
    if (iVal)
    {
	httpd->set();
	httpd_port->value( double(iVal) );
    }

    prefs.get( "props", iVal, 0 );
    if (iVal)
    {
	props->set();
	props_port->value( double(iVal) );
    }

    prefs.get( "jpg-httpd", iVal, 0 );
    if (iVal)
    {
	jpg_httpd->set();
	jpg_httpd_port->value( double(iVal) );
    }

    prefs.get( "callsign", buf, "", buflen-1 );
    callsign->value( buf );
    prefs.get( "multiplay1", buf, "", buflen-1 );
    multiplay1->value( buf );
    prefs.get( "multiplay2", buf, "", buflen-1 );
    multiplay2->value( buf );

    prefs.get( "io-count", iVal, 0 );
    for (i = 1; i <= iVal; ++i)
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("io-item-%d", i), buf, "", buflen-1 );
	io_list->add( "" );
	io_list->text( i, buf );
    }

    prefs.get( "property-count", iVal, 0 );
    for (i = 1; i <= iVal; ++i)
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("property-item-%d", i),
		   buf, "", buflen-1 );
	prop_list->add( "" );
	prop_list->text( i, buf );
    }

    prefs.get( "env-count", iVal, 0 );
    for (i = 1; i <= iVal; ++i)
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("env-var-%d", i), buf, "", buflen-1 );
	env_list->add( "" );
	env_list->text( i, buf );
    }

    prefs.get( "log-level", buf, "alert", buflen-1 );
    set_choice( log_level, buf );
    prefs.get( "trace-read-count", iVal, 0 );
    for (i = 1; i <= iVal; ++i)
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("trace-read-%d", i),
		   buf, "", buflen-1 );
	trace_read_list->add( "" );
	trace_read_list->text( i, buf );
    }
    prefs.get( "trace-write-count", iVal, 0 );
    for (i = 1; i <= iVal; ++i)
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("trace-write-%d", i),
		   buf, "", buflen-1 );
	trace_write_list->add( "" );
	trace_write_list->text( i, buf );
    }

    prefs.get( "nav1", buf, "", buflen-1 );
    nav1->value( buf );
    prefs.get( "nav2", buf, "", buflen-1 );
    nav2->value( buf );
    prefs.get( "adf", buf, "", buflen-1 );
    adf->value( buf );

    prefs.get( "dme", buf, "disabled", buflen-1 );
    if (strcmp( buf, "disabled" ) == 0)
    {
	dme->clear();
	dme_group->deactivate();
    }
    else
    {
	dme->set();
	dme_group->activate();
	dme_int_freq->deactivate();
	if (strcmp( buf, "nav1" ) == 0)
	    dme_nav1->setonly();
	else if (strcmp( buf, "nav2" ) == 0)
	    dme_nav2->setonly();
	else
	{
	    dme_int->setonly();
	    dme_int_freq->value( buf );
	}
    }

    for (i = 0; i < MAX_CLOUD_LAYERS; ++i)
    {
	prefs.get( Fl_Preferences::Name("layer-%d-elevation-ft", i),
		   cloud_elevation[i], 0.0 );
	prefs.get( Fl_Preferences::Name("layer-%d-thickness-ft", i),
		   cloud_thickness[i], 0.0 );
	prefs.get( Fl_Preferences::Name("layer-%d-transition-ft", i),
		   cloud_transition[i], 0.0 );
	prefs.get( Fl_Preferences::Name("layer-%d-span-m", i),
		   cloud_span[i], 0.0 );
	prefs.get( Fl_Preferences::Name("layer-%d-coverage", i),
		   buf, "clear", buflen-1);
	cloud_coverage[i] = string_to_coverage( buf );
    }
    cloud_layer_->value(0);
    cloud_layer_->do_callback();
}
