// advanced_funcs.cxx -- Advanced UI functions.
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

#include <FL/filename.H>
#include <FL/Fl_File_Chooser.H>

#include <simgear/misc/sg_dir.hxx>

#include "advanced.h"
#include "i18n.h"
#include "util.h"

Advanced::~Advanced()
{
}

void
Advanced::show_page( unsigned int n )
{
    for (unsigned int i = 0; i < sizeof(page)/sizeof(page[0]); ++i)
        if (i == n)
            page[i]->show();
        else
            page[i]->hide();
}

void
Advanced::list_new_cb( Fl_Browser* browser,
                            Fl_Input* input,
                            Fl_Button* del )
{
    browser->add( "" );
    browser->value( browser->size() );
    del->activate();
    input->activate();
    input->take_focus();
    input->value( "" );
}

void
Advanced::list_delete_cb( Fl_Browser* browser,
                               Fl_Input* input,
                               Fl_Button* del )
{
    int n = browser->value();
    if (n > 0)
    {
        browser->remove( n );
        input->value( "" );
        input->take_focus();
    }

    if (browser->size() == 0)
        del->deactivate();
}

void
Advanced::list_select_cb( Fl_Browser* browser,
                               Fl_Input* input,
                               Fl_Button* del )
{
    int n = browser->value();
    if (n > 0)
    {
        input->activate();
        input->value( browser->text( n ) );
        input->take_focus();
        del->activate();
    }
}

void
Advanced::list_update_cb( Fl_Browser* browser,
                               Fl_Input* input )
{
    int n = browser->value();
    if (n > 0)
    {
        browser->text( n, input->value() );
    }
}

Fl_Menu_Item Advanced::menu_time_of_day_value[] = {
 {N_("dawn"), 0,  0, (void*)"dawn", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("morning"), 0,  0, (void*)"morning", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("noon"), 0,  0, (void*)"noon", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("afternoon"), 0,  0, (void*)"afternoon", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("dusk"), 0,  0, (void*)"dusk", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("evening"), 0,  0, (void*)"evening", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("midnight"), 0,  0, (void*)"midnight", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item Advanced::menu_log_level[] = {
 {N_("bulk"), 0,  0, (void*)"bulk", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("debug"), 0,  0, (void*)"debug", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("info"), 0,  0, (void*)"info", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("warn"), 0,  0, (void*)"warn", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("alert"), 0,  0, (void*)"alert", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item Advanced::menu_cloud_coverage_[] = {
 {N_("overcast"), 0,  0, (void*)"overcast", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("broken"), 0,  0, (void*)"broken", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("scattered"), 0,  0, (void*)"scattered", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("few"), 0,  0, (void*)"few", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("cirrus"), 0,  0, (void*)"cirrus", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("clear"), 0,  0, (void*)"clear", 0, FL_NORMAL_LABEL, 0, 13, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item Advanced::menu_control[] = {
 {N_("joystick"), 0,  0, (void*)"joystick", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("keyboard"), 0,  0, (void*)"keyboard", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {N_("mouse"), 0,  0, (void*)"mouse", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item Advanced::menu_texture_filtering[] = {
 {_("no filtering"), 0,  0, (void*)"1", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {_("2.0"), 0,  0, (void*)"2", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {_("4.0"), 0,  0, (void*)"4", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {_("8.0"), 0,  0, (void*)"8", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {_("highest filtering"), 0,  0, (void*)"16", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item Advanced::menu_anti_aliasing[] = {
 {_("no anti-aliasing"), 0,  0, (void*)"1", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {_("2"), 0,  0, (void*)"2", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {_("4"), 0,  0, (void*)"4", 0, FL_NORMAL_LABEL, 0, 12, 0},
 {0,0,0,0,0,0,0,0,0}
};

void
Advanced::init()
{
    for( int i = 0; i < MAX_CLOUD_LAYERS; ++i)
    {
        cloud_elevation[i] = 0;
        cloud_thickness[i] = 0;
        cloud_coverage[i] = 0;
        cloud_transition[i] = 0;
        cloud_span[i] = 0;
    }

    for ( int i = 0; menu_time_of_day_value[i].text != 0; ++i )
    {
        menu_time_of_day_value[i].text = _( menu_time_of_day_value[i].text );
    }
    time_of_day_value->menu(menu_time_of_day_value);

    for ( int i = 0; menu_log_level[i].text != 0; ++i )
    {
        menu_log_level[i].text = _( menu_log_level[i].text );
    }
    log_level->menu(menu_log_level);

    for ( int i = 0; menu_cloud_coverage_[i].text != 0; ++i )
    {
        menu_cloud_coverage_[i].text = _( menu_cloud_coverage_[i].text );
    }
    cloud_coverage_->menu(menu_cloud_coverage_);

    for ( int i = 0; menu_control[i].text != 0; ++i )
    {
        menu_control[i].text = _( menu_control[i].text );
    }
    control->menu(menu_control);

    for ( int i = 0; menu_texture_filtering[i].text != 0; ++i )
    {
        menu_texture_filtering[i].text = _( menu_texture_filtering[i].text );
    }
    texture_filtering->menu(menu_texture_filtering);
	
    for ( int i = 0; menu_anti_aliasing[i].text != 0; ++i )
    {
        menu_anti_aliasing[i].text = _( menu_anti_aliasing[i].text );
    }
    anti_aliasing->menu(menu_anti_aliasing);

    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];
    const char* not_set = "NOT SET";
    prefs.get( "fg_root", buf, not_set, buflen-1);
    fg_root_->value( buf );
    SGPath path( fg_root_->value() );
    path.append( "Protocol" );
    simgear::Dir directory( path );
    simgear::PathList files = directory.children( simgear::Dir::TYPE_FILE | simgear::Dir::NO_DOT_OR_DOTDOT, ".xml" );
    for ( simgear::PathList::iterator ii = files.begin(); ii != files.end(); ++ii )
    {
        SGPath p(ii->file());
        io_generic_file->add(p.base().c_str());
    }
    if ( io_generic_file->size() )
        io_generic_file->value(0);

    set_choice( fdm, "jsb" );
    set_choice( log_level, "alert" );

    page_list->add(_("General"));
    page_list->add(_("Features"));
    page_list->add(_("Flight Model"));
    page_list->add(_("Freeze"));
    page_list->add(_("Initial Position"));
    page_list->add(_("Rendering"));
    page_list->add(_("Time"));
    page_list->add(_("Network"));
    page_list->add(_("Input/Output"));
    page_list->add(_("Avionics"));
    page_list->add(_("Properties"));
    page_list->add(_("Debugging"));
    page_list->add(_("Environment"));
    page_list->add(_("Weather"));
    page_list->add(_("Clouds"));

    main_window->size_range( 640, 480 );
    page_list->select(1);
    show_page(0);
}

void
Advanced::fg_browser_cb()
{
    char* p = fl_file_chooser(_("Select browser"), 0, browser->value(), 0);
    if (p != 0)
        browser->value( p );
}

int
Advanced::exec( Fl_Preferences& prefs )
{
    load_settings( prefs );
    main_window->show();

    int r;
    for (;;)
    {
        Fl_Widget* o = Fl::readqueue();
        if (o == 0) Fl::wait();
        else if (o == button[0]) { r = 0; break; }
        else if (o == button[1]) { r = 1; break; }
        else if (o == main_window) { r = 1; break; }
    }

    main_window->hide();
    if (r == 0)
        save_settings( prefs );

    return r;
}

void
Advanced::wind_dial_cb()
{
    wind_hdg->value( wind_dial->value() );
}

void
Advanced::wind_hdg_cb()
{
    wind_dial->value( wind_hdg->value() );
}

void
Advanced::cloud_layer_cb()
{
    int n = cloud_layer_->value();

    cloud_elevation_->value( cloud_elevation[n] );
    cloud_thickness_->value( cloud_thickness[n] );
    cloud_transition_->value( cloud_transition[n] );
    cloud_span_->value( cloud_span[n] );
    cloud_coverage_->value( cloud_coverage[n] );
}

void
Advanced::fg_config_cb()
{
    char* p = fl_file_chooser( _("Select a preferences file"),
                               _("XML Files(*.xml)"),
                               config->value(), 0 );
    if (p != 0)
        config->value( p );
}

void
Advanced::random_wind_cb()
{
    if (random_wind->value())
    {
        wind_hdg->deactivate();
        wind_dial->deactivate();
        wind_speed->deactivate();
    }
    else
    {
        wind_hdg->activate();
        wind_dial->activate();
        wind_speed->activate();
    }
}

void
Advanced::ai_models_cb()
{
    if ( ai_models->value() == 0 )
    {
        ai_traffic->value(0);
        ai_traffic->deactivate();
    }
    else
    {
        ai_traffic->activate();
    }
}
