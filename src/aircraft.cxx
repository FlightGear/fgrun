// aircraft.cxx -- Aircraft search and update functions.
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

#ifdef _MSC_VER
# pragma warning(disable: 4786)
# define snprintf _snprintf
#endif

#include <string.h>
#include <FL/Fl.h>
#include <FL/filename.h>

#include "UserInterface.h"

void
UserInterface::update_aircraft()
{
    aircraft->clear();
    char buf[FL_PATH_MAX];
    snprintf( buf, sizeof(buf), "%s/Aircraft/", fg_root->value() );
    dirent** files;

    // Search $FG_ROOT/Aircraft directory.

    int num_files = fl_filename_list( buf, &files, fl_numericsort );
    if (num_files < 0)
	return;

    for (int i = 0; i < num_files; ++i)
    {
	if (fl_filename_match(files[i]->d_name, "*-set.xml"))
	{
	    // Extract aircraft name from filename.
	    char* p = strstr( files[i]->d_name, "-set.xml" );
	    if (p != 0) *p = 0;
	    int index = aircraft->add( files[i]->d_name, 0, 0, 0, 0);
	    
	    if (strcmp(files[i]->d_name, default_aircraft.c_str()) == 0)
		aircraft->value(index);

	}
	free( files[i] );
    }

    free( files );
}
