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

#include <iostream>
#include <deque>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

#include <FL/Fl.h>
#include <FL/filename.h>
#include <FL/fl_ask.h>

#include "UserInterface.h"
#include "airportdb.h"

using std::deque;
using std::string;
using std::vector;

/**
 * FIFO queue of directory names in which to search for airport files.
 */
static deque< string > apt_dirs;

/**
 * Array of (sorted) airport ICAO identifiers.
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
    apt_browser->clear();

    if (apt_show_installed->value())
    {
	int count = airport->size() - 1;
	for (int i = 0; i < count; ++i)
	{
	    const apt_dat_t* apt = airportdb_->find( airport->text(i) );
	    if (apt != 0)
	    {
		char buf[ 80 ];
		snprintf( buf, sizeof buf, "@f%-4.4s\t%-s",
			  apt->id_.c_str(), apt->name_.c_str() );
		apt_browser->add( buf );
	    }
	}
    }
    else
    {
	AirportDB::const_iterator first( airportdb_->begin() ); 
	AirportDB::const_iterator last( airportdb_->end() ); 
	for (; first != last; ++first)
	{
	    char buf[ 80 ];
	    snprintf( buf, sizeof buf, "@f%-4.4s\t%-s",
		      first->id_.c_str(), first->name_.c_str() );
	    apt_browser->add( buf );
	}
    }
}

void
UserInterface::apt_browser_cb()
{
}

void
UserInterface::apt_id_cb()
{
    string id( apt_id->value() );
    std::transform( id.begin(), id.end(), id.begin(), toupper );

    if (apt_show_installed->value())
    {
    }
    else
    {
	int i = airportdb_->ifind( id.c_str() );
	if (i >= 0)
	    apt_browser->select(i+1);
    }

}

void
UserInterface::apt_name_cb()
{
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
    airportdb_ = new AirportDB;
    if (!load_airports_->value())
	return;

    char buf[FL_PATH_MAX];
    snprintf( buf, sizeof(buf), "%s/Airports/runways.dat.gz",
	      fg_root->value() );

    try
    {
	airportdb_->load( buf );
    }
    catch (const char* msg)
    {
	fl_alert( "Error loading airport database: %s", msg );
	//delete airportdb_;
    }
}
