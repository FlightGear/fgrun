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

#include <cstdio>
#include <fstream>
#include <FL/filename.h>
#include <FL/fl_ask.h>

#if defined(HAVE_STRING_H)
#include <string.h>	// strcmp
#endif

#include "UserInterface.h"

using std::ofstream;

void
UserInterface::run_fgfs()
{
    if (write_fgfsrc())
	run_fgfs_impl();
}

int
UserInterface::write_fgfsrc()
{
    char fname[ FL_PATH_MAX ];
#if defined(_MSC_VER)
    _snprintf( fname, sizeof(fname), "%s\\\\system.fgfsrc", fg_root->value() );
#else
    fl_filename_expand( fname, "~/.fgfsrc" );
#endif

    FILE* fp = fopen( fname, "r" );
    if (fp != 0)
    {
	fclose( fp );
	int r = fl_ask( "About to overwrite %s.\nDo you want to continue?",
			fname );
	if (!r)
	    return 0;
    }

    ofstream ofs( fname );
    if (ofs) {
	ofs << "--fg-root=" << fg_root->value()
	    << "\n--fg-scenery=" << fg_scenery->value();

	// Only write non-default options.

	// General options.
	if (airport->text() && strcmp(airport->text(), "KSFO") != 0)
	    ofs << "\n--airport-id=" << airport->text();
	if (runway->value() > 1)
	    ofs << "\n--runway=" << runway->text();
	if (aircraft->text() && strcmp(aircraft->text(), "c172") != 0)
	    ofs << "\n--aircraft=" << aircraft->text();
	if (strcmp(control->text(), "joystick") != 0)
	    ofs << "\n--control=" << control->text();
	if (lang->size() > 0)
	    ofs << "\n--lang=" << lang->value();
	if (browser->size() > 0)
	    ofs << "\n--browser-app=" << browser->value();
  
	// Features
	if (game_mode->value())
	    ofs << "\n--enable-game-mode";
	if (!splash_screen->value())
	    ofs << "\n--disable-splash-screen";
	if (!intro_music->value())
	    ofs << "\n--disable-intro-music";
	// ?mouse_pointer?
	if (random_objects->value())
	    ofs << "\n--enable-random-objects";
	else
	    ofs << "\n--disable-random-objects";
	if (!panel->value())
	    ofs << "\n--disable-panel";
	if (!sound->value())
	    ofs << "\n--disable-sound";
	if (hud->value()) {
	    ofs << "\n--enable-hud";
	    if (antialias_hud->value())
		ofs << "\n--enable-anti-alias-hud";
	    else
		ofs << "\n--disable-anti-alias-hud";
	}
	if (!hud_3d->value())
	    ofs << "\n--disable-hud-3d";
	if (auto_coordination->value())
	    ofs << "\n--enable-auto-coordination";
	if (horizon_effect->value())
	    ofs << "\n--enable-horizon-effect";
	if (enhanced_lighting->value())
	    ofs << "\n--enable-enhanced-lighting";
	if (distance_attenuation->value())
	    ofs << "\n--enable-distance-attenuation";
	if (!specular_highlight->value())
	    ofs << "\n--disable-specular-highlight";

	if (failure->value())
	{
	    if (failure_pitot->value())
		ofs << "\n--failure=pitot";
	    if (failure_static->value())
		ofs << "\n--failure=static";
	    if (failure_system->value())
		ofs << "\n--failure=system";
	    if (failure_vacuum->value())
		ofs << "\n--failure=vacuum";
	}

	// Flight model
	if (strcmp(fdm->text(), "jsb" ) != 0)
	    ofs << "\n--fdm=" << fdm->text();
	else if (notrim->value())
	    ofs << "\n--notrim";
	if (model_hz->value() != 120.0)
	    ofs << "\n--model_hz=" << model_hz->value();
	if (speed->value() != 1)
	    ofs << "\n--speed=" << speed->value();
	if (in_air->value())
	    ofs << "\n--in-air";
	if (wind->size() > 0)
	    ofs << "\n--wind=" << wind->value();
	if (turbulence->value() > 0)
	    ofs << "\n--turbulence=" << turbulence->value();
	if (ceiling->size() > 0)
	    ofs << "\n--ceiling=" << ceiling->value();

	// Freeze
	if (freeze->value())
	    ofs << "\n--enable-freeze";
	if (fuel_freeze->value())
	    ofs << "\n--enable-fuel-freeze";
	if (clock_freeze->value())
	    ofs << "\n--enable-clock-freeze";
	
	// Initial position and orientation.
	if (lon->size() > 0)
	    ofs << "\n--lon=" << lon->value();
	if (lat->size() > 0)
	    ofs << "\n--lat=" << lat->value();
	if (altitude->size() > 0)
	    ofs << "\n--altitude=" << altitude->value();
	if (heading->value() != 0.0)
	    ofs << "\n--heading=" << heading->value();
	if (roll->value() != 0.0)
	    ofs << "\n--roll=" << roll->value();
	if (pitch->value() != 0.0)
	    ofs << "\n--pitch=" << pitch->value();
	if (vc->size() > 0)
	    ofs << "\n--vc=" << vc->value();
	if (uBody->size() > 0)
	    ofs << "\n--uBody=" << uBody->value();
	if (vBody->size() > 0)
	    ofs << "\n--vBody=" << vBody->value();
	if (wBody->size() > 0)
	    ofs << "\n--wBody=" << wBody->value();

	// Rendering.
	if (!clouds->value())
	    ofs << "\n--disable-clouds";
	if (clouds3d->value())
	    ofs << "\n--enable-clouds3d";
	if (fullscreen->value())
	    ofs << "\n--enable-fullscreen";
	if (!skyblend->value())
	    ofs << "\n--disable-skyblend";
	if (!textures->value())
	    ofs << "\n--disable-textures";
	if (wireframe->value())
	    ofs << "\n--enable-wireframe";
	if (shading_flat->value())
	    ofs << "\n--shading-flat";
	if (fog_disabled->value())
	    ofs << "\n--fog-disable";
	else if (fog_fastest->value())
	    ofs << "\n--fog-fastest";
	if (geometry->size() > 0)
	    ofs << "\n--geometry=" << geometry->value();
	if (visibility->size() > 0)
	{
	    if (vis_meters->value())
		ofs << "\n--visibility=" << visibility->value();
	    else
		ofs << "\n--visibility-miles=" << visibility->value();
	}

	// ?view-offset?
	if (strcmp(bpp->text(), "16") != 0)
	    ofs << "\n--bpp=" << bpp->text();
	if (fov->value() != 60.0)
	    ofs << "\n--fov=" << fov->value();

	// Time
	if (time_match_real->value() && time_offset_value->size() > 0)
	    ofs << "\n--time-offset=" << time_offset_value->value();
	else if (time_match_local->value())
	    ofs << "\n--time-match-local";
	else if (start_date_sys->value() && start_date_sys_value->size() > 0)
	    ofs << "\n--start-date-sys=" << start_date_sys_value->value();
	else if (start_date_gmt->value() && start_date_gmt_value->size() > 0)
	    ofs << "\n--start-date-gmt=" << start_date_gmt_value->value();
	else if (start_date_lat->value() && start_date_lat_value->size() > 0)
	    ofs << "\n--start-date-lat=" << start_date_lat_value->value();
	else if (time_of_day->value())
	    ofs << "\n--timeofday=" << time_of_day_value->text();

	// Network.
	if (httpd->value())
	    ofs << "\n--httpd=" << int(httpd_port->value());
	if (props->value())
	    ofs << "\n--props=" << int(props_port->value());
	if (jpg_httpd->value())
	    ofs << "\n--jpg_httpd=" << int(jpg_httpd_port->value());
	if (network_olk->value()) {
	    ofs << "\n--enable-network-olk";
	    if (net_hud->value())
		ofs << "\n--net-hud";
	    if (net_id->size() < 0)
		ofs << "\n--net-id=" << net_id->value();
	}

	int i;

	// I/O options.
	for (i = 1; i <= io_list->size(); ++i)
	{
	    ofs << "\n" << io_list->text(i);
	}

	// Properties
	for (i = 1; i <= prop_list->size(); ++i)
	{
	    ofs << "\n--prop:" << prop_list->text(i);
	}

	// Debugging
	ofs << "\n--log-level=" << log_level->text();
	for (i = 1; i <= trace_read_list->size(); ++i)
	{
	    ofs << "\n--trace-read=" << trace_read_list->text(i);
	}
	for (i = 1; i <= trace_write_list->size(); ++i)
	{
	    ofs << "\n--trace-write=" << trace_write_list->text(i);
	}

	// Avionics
	if (nav1->size() > 1)
	    ofs << "\n--nav1=" << nav1->value();
	if (nav2->size() > 1)
	    ofs << "\n--nav2=" << nav2->value();
	if (adf->size() > 1)
	    ofs << "\n--adf=" << adf->value();
	if (dme->value())
	{
	    if (dme_nav1->value())
		ofs << "\n--dme=nav1";
	    else if (dme_nav2->value())
		ofs << "\n--dme=nav2";
	    else if (dme_int->value())
		ofs << "\n--dme=" << dme_int_freq->value();
	}

	ofs << "\n";
	ofs.close();
	return 1;
    }

    return 0;
}
