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

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include <string.h>
#include <FL/Fl.h>
#include <FL/filename.h>

#include "UserInterface.h"

using std::vector;
using std::string;

struct Comp
{
    const string& s_;
    Comp( const string& s ) : s_(s) {}

    bool operator()( const string& a ) {
	return s_.compare( 0, a.length(), a ) == 0;
    }
};

void
UserInterface::update_aircraft()
{
    aircraft->clear();
    char buf[FL_PATH_MAX];
    snprintf( buf, sizeof(buf), "%s/Aircraft/", fg_root->value() );
    dirent** files;

    // Search $FG_ROOT/Aircraft directory.

    int num_files = fl_filename_list( buf, &files, fl_casenumericsort );
    if (num_files < 0)
	return;

    typedef vector< string > string_vec_t;
    typedef string_vec_t::iterator SVI;

    string_vec_t ac;
    for (int i = 0; i < num_files; ++i)
    {
	if (fl_filename_match(files[i]->d_name, "*-set.xml"))
	{
	    // Extract aircraft name from filename.
	    char* p = strstr( files[i]->d_name, "-set.xml" );
	    if (p != 0) *p = 0;

	    ac.push_back( string( files[i]->d_name ) );

	}
	free( files[i] );
    }
    free( files );

    string_vec_t submenus;
    // Hardcoded aircraft submenus.
    submenus.push_back( string( "c172" ) );
    submenus.push_back( string( "c310" ) );
    submenus.push_back( string( "j3cub" ) );
    //std::sort( submenus.begin(), submenus.end() );

    // Create submenus for some aircraft types.
    SVI first( ac.begin() );
    SVI last( ac.end() );
    int index;
    for (; first != last; ++first)
    {
	SVI i( find_if( submenus.begin(), submenus.end(), Comp(*first) ) );
	if (i != submenus.end())
	{
	    string s( *i );
	    s.append( "/" );
	    s.append( *first );
	    index = aircraft->add( s.c_str(), 0, 0, 0, 0);
	}
	else
	{
	    index = aircraft->add( first->c_str(), 0, 0, 0, 0);
	}

	if (*first == default_aircraft)
	    aircraft->value(index);
   }
}

