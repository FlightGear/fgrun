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
#endif

#include <sstream>
#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iterator>
#include <utility>
#include <iostream>

#include <FL/Fl.h>
#include <FL/filename.h>
#include <FL/fl_ask.h>

#include "UserInterface.h"
#include "airportdb.h"

using std::deque;
using std::string;
using std::vector;
using std::ostringstream;

void load_airportdb_cb( void* v );

/**
 * FIFO queue of directory names in which to search for airport files.
 */
static deque< string > apt_dirs;

/**
 * Installed airport ICAO identifiers.
 */
typedef vector<string> vs_t;
typedef vs_t::iterator vsi_t;

static vs_t airports;

static bool
comp( const string& a, const string& b )
{
    string::size_type len = std::min( a.length(), b.length() );
    return a.compare( 0, 1, b ) < 0;
}

static bool
id_comp( const apt_dat_t* a, const apt_dat_t* b )
{
    return a->id_ == b->id_;
}

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

	    vsi_t first = airports.begin();
	    vsi_t last = airports.end();
	    int index = 0;

	    if (airports.size() > (26+10))
	    {
		for (; first != last; ++first)
		{
		    string s = first->substr( 0, 1 );
		    s.append( "/" );
		    s.append( *first );
		    index = ui->airport->add( s.c_str() );
		    if (ui->default_airport == *first)
			ui->airport->value(index);
		}
	    }
	    else
	    {
		for (; first != last; ++first)
		{
		    index = ui->airport->add( first->c_str() );
		    if (ui->default_airport == *first)
			ui->airport->value(index);
		}
	    }

	    ui->airport->activate();
	    ui->update_runways();
	    ui->load_airport_browser();
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
	Fl::add_idle( load_airportdb_cb, this );
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

    ostringstream buf;
    buf << std::setw(2) << std::setfill('0') << heading << c;
    return buf.str();
}

void
UserInterface::update_runways()
{
    runway->clear();
    runway->add( "<default>" );
    runway->activate();
    runway->value(0);

    const apt_dat_t* apt = airportdb_->find( airport->text() );
    if (apt == 0)
    {
	return;
    }

    typedef vector< string > vs_t;
    vs_t::const_iterator first( apt->runways_.begin() );
    vs_t::const_iterator last( apt->runways_.end() );
    for (; first != last; ++first)
    {
	runway->add( first->c_str() );
	string rrwy = reverse_runway( *first );
	if (!rrwy.empty())
	    runway->add( rrwy.c_str() );
    }
}

void
UserInterface::load_airport_browser()
{
    std::vector< const apt_dat_t* > apts;

    if (apt_show_installed->value())
    {
	vs_t::size_type count = airports.size();
	apts.reserve( count );
	
	for (int i = 0; i < count; ++i)
	{
	    const apt_dat_t* apt = airportdb_->find( airports[i].c_str() );
	    if (apt != 0)
	    {
		apts.push_back( apt );
	    }
	}

	apt_browser->set_airports( apts );
    }
    else
    {
	static std::vector< const apt_dat_t* > all_apts;

	if (all_apts.empty())
	{
	    all_apts.reserve( airportdb_->size() );
	    AirportDB::const_iterator first( airportdb_->begin() ); 
	    AirportDB::const_iterator last( airportdb_->end() ); 
	    for (; first != last; ++first)
	    {
		all_apts.push_back( &*first );
	    }

	    // Remove duplicate airport ids.
	    all_apts.erase( std::unique( all_apts.begin(),
					 all_apts.end(), id_comp ),
			    all_apts.end() );
	}
	apt_browser->set_airports( all_apts );
    }
}

void
UserInterface::apt_browser_cb()
{
}

void
UserInterface::apt_id_cb()
{
    // Convert to uppercase.
    const char* s = apt_id->value();
    for (int i = 0; s[i] != 0; ++i)
    {
	if (islower(s[i]))
	{
	    char c = toupper( s[i] );
	    apt_id->replace( i, i+1, &c, 1 );
	}
    }

    apt_browser->select_id( s );
}

void
UserInterface::apt_name_cb()
{
    apt_browser->select_name( apt_name->value() );
}

void
load_airportdb_cb( void* v )
{
    ((UserInterface*)v)->load_airportdb();
    Fl::remove_idle( load_airportdb_cb, v );
}

void
UserInterface::load_airportdb()
{
    ostringstream buf;
    buf << fg_root->value() << "/Airports/runways.dat.gz";

    try
    {
	airportdb_->load( buf.str().c_str() );
    }
    catch (const char* msg)
    {
	fl_alert( "Error loading airport database: %s", msg );
    }
}
