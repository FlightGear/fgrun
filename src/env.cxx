// env.cxx -- Env option browser functions.
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

#if defined(_MSC_VER)
#define snprintf _snprintf
#endif

void
UserInterface::env_new_cb()
{
    env_list->add( "" );
    env_list->value( env_list->size() );
    env_delete->activate();
    env_var->activate();
    env_var->take_focus();
    env_var->value( "" );
}

void
UserInterface::env_delete_cb()
{
}

void
UserInterface::env_list_select_cb( Fl_Browser* o )
{
    int n = o->value();
    if (n > 0)
    {
    }
}

void
UserInterface::env_list_update_cb()
{
    int n = env_list->value();
    if (n > 0)
    {
	env_list->text( n, env_var->value() );
    }
}

