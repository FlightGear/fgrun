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

#include "advanced.h"

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

void
Advanced::init()
{
    page_list->add("General");
    page_list->add("Features");
    page_list->add("Flight Model");
    page_list->add("Freeze");
    page_list->add("Initial Position");
    page_list->add("Rendering");
    page_list->add("Time");
    page_list->add("Network");
    page_list->add("Input/Output");
    page_list->add("Avionics");
    page_list->add("Properties");
    page_list->add("Debugging");
    page_list->add("Environment");
    page_list->add("Weather");
    page_list->add("Clouds");

    page_list->select(1);
    show_page(0);
}

void
Advanced::fg_browser_cb()
{
    char* p = fl_file_chooser("Select browser", 0, browser->value(), 0);
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
    char* p = fl_file_chooser( "Select a preferences file",
			       "XML Files(*.xml)",
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
