// util.cxx -- Utility functions.
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

#include "UserInterface.h"

UserInterface::~UserInterface()
{
}

void
UserInterface::show_page( unsigned int n )
{
    for (unsigned int i = 0; i < sizeof(page)/sizeof(page[0]); ++i)
	if (i == n)
	    page[i]->show();
	else
	    page[i]->hide();
}

int
UserInterface::set_choice( Fl_Choice* c, const char* s )
{
    const Fl_Menu_Item* m = c->menu();
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
	    if (strcmp(s, m[i].label()) == 0)
	    {
		c->value(i);
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
UserInterface::list_new_cb( Fl_Browser* browser,
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
UserInterface::list_delete_cb( Fl_Browser* browser,
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
UserInterface::list_select_cb( Fl_Browser* browser,
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
UserInterface::list_update_cb( Fl_Browser* browser,
			       Fl_Input* input )
{
    int n = browser->value();
    if (n > 0)
    {
	browser->text( n, input->value() );
    }
}

