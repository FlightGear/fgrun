#include <iostream>
#include <algorithm>
#include <zlib.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Progress.H>

#include "airportdb.h"

using std::vector;
using std::string;

static bool
apt_id_comp( const apt_dat_t& a, const apt_dat_t& b )
{
    return a.id_ < b.id_;
}

AirportDB::AirportDB()
{
}

//012345678901234567
//A KSFO    13 CYN San Francisco Intl
//R KSFO 10L  37.621134 -122.375264 117.90 11845   200 YAPHN NYPN...

void
AirportDB::load( const char* fname )
{
    airports_.clear();
    airports_.reserve( 22705 );

    gzFile gzf = gzopen( fname, "rb" );
    if (gzf == 0)
    {
	throw "gzopen error";
    }

#if 1
    Fl_Window* win = new Fl_Window( 300, 100 );
    Fl_Progress* progress = new Fl_Progress( 10, 30, 280, 25,
					     "Loading airport database" );
    win->end();
    win->set_modal();
    win->show();
#endif

    // Skip first line.
    char c;
    while ((c = gzgetc(gzf)) != -1 && c != '\n')
	;

    int count = 0;
    char line[128];
    while (gzgets( gzf, line, sizeof line ) != 0)
    {
	if (line[0] == 'A' || line[0] == 'H' || line[0] == 'S')
	{
	    ++count;
	    string id( line+2, 4 );
	    if (id[3] == ' ')
		id.erase(3);

	    string name( line+17 );
	    if (name[name.size()-1] == '\n')
		name.erase( name.size()-1 );

	    apt_dat_t apt;
	    apt.id_ = id;
	    apt.name_ = name;
	    airports_.push_back( apt );
	}
	else if (line[0] == 'R')
	{
	    string rwy( line+7, 3 );
	    if (rwy[2] == ' ')
		rwy.erase(2);

	    airports_[ airports_.size() - 1 ].runways_.push_back( rwy );
	}

	if ((count % 200) == 0)
	{
	    float value = float(count) * 100.f / 22705.f;
	    progress->value(value);
	    Fl::check();
	}
    }

    gzclose( gzf );
    win->hide();
    delete win;

    std::sort( airports_.begin(), airports_.end(), apt_id_comp );
}

const apt_dat_t*
AirportDB::find( const char* id ) const
{
    apt_dat_t key;
    key.id_ = string(id);
    const_iterator i = std::lower_bound( airports_.begin(),
					 airports_.end(),
					 key,
					 apt_id_comp );
    return i == airports_.end() ? 0 : &*i;
}
