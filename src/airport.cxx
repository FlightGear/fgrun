// airport.cxx -- Airport search and update functions.
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

#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

#include <FL/Fl.h>
#include <FL/filename.h>

#include <mk4.h>

#include "UserInterface.h"

using std::deque;
using std::string;
using std::vector;

/**
 * FIFO queue of directory names in which to search for airport files.
 */
static deque< string > apt_dirs;

/**
 * Array of (sorted) airport names.
 */
static vector< string > airports;

/**
 * 
 */
void
search_for_airports_cb( void* v )
{
    string dir = apt_dirs.front();
    dirent** files;
    int n = fl_filename_list( dir.c_str(), &files, fl_numericsort );
    if (n > 0)
    {
	for (int i = 0; i < n; ++i)
	{
	    if (fl_filename_match( files[i]->d_name,
				   "[ew][0-9][0-9][0-9][ns][0-9][0-9]*")) {
		// Found a scenery sub-directory.
		string d = dir;
		d += "/";
		d += files[i]->d_name;
		if (fl_filename_isdir( d.c_str() )) {
		    apt_dirs.push_back( d );
		}
	    }
	    else if (fl_filename_match( files[i]->d_name, "???.btg.gz") ||
		     fl_filename_match( files[i]->d_name, "????.btg.gz"))
	    {
		char* p = strstr( files[i]->d_name, ".btg" );
		if (p != 0)
		    *p = 0;
		airports.push_back( string( files[i]->d_name ) );
	    }

	    free( files[i] );
	}

	free( files );
    }

    apt_dirs.pop_front();
    if (apt_dirs.empty())
    {
	Fl::remove_idle( search_for_airports_cb, v );
	UserInterface* ui = (UserInterface*)v;
	ui->airport->clear();
	//ui->airport->add( "<default>" );

	if (!airports.empty())
	{
	    std::sort( airports.begin(), airports.end() );
	    typedef vector<string> StringVec;
	    StringVec::iterator i = airports.begin();
	    StringVec::iterator end = airports.end();
	    int index = 0;
	    for (; i != end; ++i)
	    {
		index = ui->airport->add( i->c_str() );
		if (ui->default_airport == *i)
		    ui->airport->value(index);
	    }
	    ui->airport->activate();
	    ui->update_runways();
	}
    }
}

/**
 * 
 */
void
UserInterface::update_airports_cb()
{
    if (fg_scenery->size() > 0
	&& fl_filename_isdir(fg_scenery->value()))
    {
	airports.clear();
	string dir = fg_scenery->value();
	dir += "/";
	apt_dirs.push_back( dir );
	Fl::add_idle( search_for_airports_cb, this );
    }
}

/**
 * 
 */
static string
reverse_runway( const string& rwy )
{
    if (!isdigit( rwy[0] ))
	return string("");

    int heading = atoi( rwy.c_str() );
    heading += 18;
    if (heading > 36)
	heading -= 36;
    char c = rwy[ rwy.length() - 1 ];
    if (c == 'L')
	c = 'R';
    else if (c == 'R')
	c = 'L';
    else if (c != 'C')
	c = 0;

    char buf[8];
    snprintf( buf, sizeof(buf)-1, "%0d%c", heading, c );
    return string(buf);
}

void
UserInterface::update_runways()
{
    vector< string > rwys;

    string s = fg_root->value();
    s += "/Airports/runways.mk4";
    c4_Storage storage( s.c_str(), false );
    
    c4_View view;
    view = storage.GetAs( "runway[ID:S,Rwy:S,Longitude:F,Latitude:F,Heading:F,Length:F,Width:F,SurfaceFlags:S,End1Flags:S,End2Flags:S]" );

    runway->clear();
    runway->add( "<default>" );

    c4_StringProp pID( "ID" );
    c4_StringProp pRwy( "Rwy" );
    string apt_id = airport->text();
    int i = view.Find( pID[ apt_id.c_str() ] );
    if (i == -1)
    {
	return;
    }

    c4_RowRef row = view.GetAt( i );
    string id = (const char*) pID(row);
    string rwy = (const char*) pRwy(row);
    while (id == apt_id)
    {
	rwys.push_back( rwy );
	string rrwy = reverse_runway( rwy );
	if (!rrwy.empty())
	    rwys.push_back( rrwy );

	++i;
	row = view.GetAt( i );
	id = (const char*) pID( row );
	rwy = (const char*) pRwy(row);
    }
    
    typedef vector<string> vs_type;
    vs_type::iterator first = rwys.begin();
    vs_type::iterator last = rwys.end();
    while (first != last)
    {
	runway->add( first->c_str() );
	++first;
    }

    runway->activate();
    runway->value(0);
}
