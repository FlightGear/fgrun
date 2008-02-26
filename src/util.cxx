// util.cxx -- Utility functions.
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

#include <FL/Fl_Choice.H>
#include <cstring>
#include "i18n.h"

int
set_choice( Fl_Choice* c, const char* s )
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
	    if (strcmp(_(s), m[i].label()) == 0)
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

int
set_choice_from_data( Fl_Choice* c, const char* s )
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
	else if (m[i].user_data() != 0)
	{
	    if (strcmp(s, (const char *)m[i].user_data()) == 0)
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
