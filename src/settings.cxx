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
UserInterface::default_settings_cb()
{
}
