// props.cxx -- Properties page callbacks.
//
// Written by Bernie Bright, started May 2003.
//
// Copyright (c) 2003  Bernie Bright - bbright@bigpond.net.au
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

void
UserInterface::prop_new_cb()
{
    prop_list->add( "" );
    prop_list->value( prop_list->size() );
    prop_delete->activate();
    prop_text->activate();
    prop_text->take_focus();
    prop_text->value( "" );
}

void
UserInterface::prop_delete_cb()
{
    int n = prop_list->value();
    if (n > 0)
    {
	prop_list->remove( n );
	prop_text->value( "" );
	prop_text->take_focus();
    }
}

void
UserInterface::prop_list_select_cb( Fl_Browser* o )
{
    int n = o->value();
    if (n > 0)
    {
	prop_text->activate();
	prop_text->value( o->text( n ) );
	prop_text->take_focus();
	prop_delete->activate();
    }
}

void
UserInterface::prop_list_update_cb()
{
    int n = prop_list->value();
    if (n > 0)
    {
	prop_list->text( n, prop_text->value() );
    }
}
