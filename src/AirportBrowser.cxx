// AirportBrowser.cxx -- Airport and runway browser widget.
//
// Written by Bernie Bright, started Oct 2003.
//
// Copyright (c) 2003  Bernie Bright - bbright@users.sourceforge.net
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

#include <algorithm>
#include <deque>
#include <iterator>

#include <FL/filename.H>
#include <FL/Fl_Round_Button.H>

#include "AirportBrowser.h"
#include "AirportTable.h"

using std::string;
using std::vector;
using std::deque;

AirportBrowser::AirportBrowser( int X, int Y, int W, int H,
				const char* l )
    : Fl_Group( X, Y, W, H, l )
    , gzf_(0)
    , airports_loaded(false)
{
    {
// 	Fl_Group* g = new Fl_Group( X, Y, W, 25, "Show:" );
// 	g->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
// 	//g->box( FL_ENGRAVED_BOX );
// 	Fl_Round_Button* o;
//  	o = new Fl_Round_Button( 60, Y, 80, 25, "All" );
// 	o->type( FL_RADIO_BUTTON );
// 	o->callback( show_all_cb, this );

//  	o = new Fl_Round_Button( X+140, Y, 80, 25, "Installed" );
//  	o->type( FL_RADIO_BUTTON );
// 	o->callback( show_installed_cb, this );
// 	o->setonly();
// 	g->end();
    }

    Y += 5;
    int tw = W - 120 - 5;
    int th = H - 35 - 5;

    table_ = new AirportTable( X, Y, tw, th );
    table_->color( FL_LIGHT3 );
    table_->selection_color( (Fl_Color)3 );
    table_->labeltype( FL_NO_LABEL );
    table_->labelfont(0);
    table_->labelsize(14);
    table_->labelcolor( FL_BLACK );
    table_->callback( browser_cb, this );
    table_->when( FL_WHEN_RELEASE );
    table_->end();

    id_ = new Fl_Input( X, Y+th+5, 75, 25 );
    id_->textsize( 12 );
    id_->callback( id_cb, this );
    id_->when( FL_WHEN_CHANGED );

    name_ = new Fl_Input( X+75+5, Y+th+5, tw-75-20, 25 );
    name_->textsize( 12 );
    name_->callback( name_cb, this );
    name_->when( FL_WHEN_CHANGED );

    runways_ = new Fl_Hold_Browser( X+tw+5, Y, 120, th, "Runways" );
    runways_->align( FL_ALIGN_TOP );
}

AirportBrowser::~AirportBrowser()
{
}

void
AirportBrowser::draw()
{
    Fl_Group::draw();
}

int
AirportBrowser::handle( int e )
{
    return Fl_Group::handle( e );
}

void
AirportBrowser::col_header_cb( Fl_Widget* o, void* v )
{
    AirportTable* table = ((AirportTable*)v);
    table->col_header_cb( table->callback_col() );
}

void
AirportBrowser::id_cb( Fl_Widget* o, void* v )
{
    ((AirportBrowser*)v)->id_cb();
}

void
AirportBrowser::id_cb()
{
    // Convert to uppercase.
    const char* s = id_->value();
    for (int i = 0; s[i] != 0; ++i)
    {
	if (islower( s[i] ))
	{
	    char c = toupper( s[i] );
	    id_->replace( i, i+1, &c, 1 );
	}
    }

    const apt_dat_t* apt = table_->select_id( s );
    show_runways( apt );
}

void
AirportBrowser::name_cb( Fl_Widget* o, void* v )
{
    ((AirportBrowser*)v)->name_cb();
}

void
AirportBrowser::name_cb()
{
    table_->select_name( name_->value() );
}

void
AirportBrowser::show_all_cb( Fl_Widget* o, void* v )
{
    ((AirportBrowser*)v)->show_all();
}

void
AirportBrowser::show_installed_cb( Fl_Widget* o, void* v )
{
    ((AirportBrowser*)v)->show_installed();
}

Fl_Color
AirportBrowser::col_header_color() const
{
    return table_->col_header_color();
}

void
AirportBrowser::col_header_color( Fl_Color c )
{
    table_->col_header_color( c );
}

static bool
apt_id_comp( const apt_dat_t& a, const apt_dat_t& b )
{
    return a.id_ < b.id_;
}

/**
 * 
 */
void
AirportBrowser::idle_proc( void* v )
{
    ((AirportBrowser*)v)->idle_proc();
}

void
AirportBrowser::idle_proc( )
{
    int count = 200;
    char line[128];
    while (count-- > 0 && gzgets( gzf_, line, sizeof line ) != 0)
    {
	// Read 'count' airports, or until EOF or error.
        if (line[0] == 'A' || line[0] == 'H' || line[0] == 'S')
        {
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

	    airports_.back().runways_.push_back( rwy );
        }
    }

    if (gzeof( gzf_ ))
    {
	gzclose( gzf_ );
	Fl::remove_idle( idle_proc, this );
	std::sort( airports_.begin(), airports_.end(), apt_id_comp );
	show_installed();
    }
}

const apt_dat_t*
AirportBrowser::find( const string& id ) const
{
    apt_dat_t key;
    key.id_ = id;
    vector< apt_dat_t >::const_iterator
	i( std::lower_bound( airports_.begin(), airports_.end(),
			     key, apt_id_comp ) );
    return i == airports_.end() ? 0 : &*i;
}

void
AirportBrowser::scan_installed_airports( const string& dir )
{
    if (!fl_filename_isdir( dir.c_str() ))
	return;

    deque< string > dirs;
    dirs.push_back( dir );

    installed_airports_.clear();

    do
    {
	string cwd( dirs.front() );
	dirent** files;
	int n = fl_filename_list( cwd.c_str(), &files, fl_numericsort );
	if (n > 0)
	{
	    for (int i = 0; i < n; ++i)
	    {
		if (fl_filename_match( files[i]->d_name,
			       "[ew][0-9][0-9][0-9][ns][0-9][0-9]*"))
		{
		    // Found a scenery dub-directory.
		    string d(cwd);
		    d += "/";
		    d += files[i]->d_name;
		    if (fl_filename_isdir( d.c_str() ) )
		    {
			dirs.push_back( d );
		    }
		}
		else if (fl_filename_match( files[i]->d_name,
					    "???.btg.gz" ) ||
			 fl_filename_match( files[i]->d_name,
					    "????.btg.gz" ))
		{
		    char* p = strstr( files[i]->d_name, ".btg" );
		    if (p != 0)
			*p = 0;
		    installed_airports_.push_back(
					  string( files[i]->d_name ) );
		}

		free( files[i] );
	    }

	    free( files );
	}

	dirs.pop_front();
    }
    while (!dirs.empty());

    std::sort( installed_airports_.begin(), installed_airports_.end() );
    // Remove duplicate airports.
    installed_airports_.erase( std::unique( installed_airports_.begin(),
					    installed_airports_.end() ),
			       installed_airports_.end() );
}

void
AirportBrowser::show_all()
{
}

void
AirportBrowser::show_installed()
{
    static vector<const apt_dat_t*> apts;
    typedef vector<string>::size_type size_type;

    if (apts.empty())
    {
	size_type count = installed_airports_.size();
	apts.reserve( count );
	for (size_type i = 0; i < count; ++i)
	{
	    const apt_dat_t* apt = find( installed_airports_[i] );
	    if (apt != 0)
		apts.push_back( apt );
	}
    }

    table_->set_airports( apts );
}

void
AirportBrowser::init( const string& fg_root, const string& fg_scenery )
{
    string fname( fg_root );
    fname += "/Airports/runways.dat.gz";
    airports_.clear();
    airports_.reserve( 27000 );

    gzf_ = gzopen( fname.c_str(), "rb" );
    if (gzf_ == 0)
    {
        throw "gzopen error";
    }

    // Skip first line.
    char c;
    while ((c = gzgetc(gzf_)) != -1 && c != '\n')
        ;

    // Load the file in the background.
    Fl::add_idle( idle_proc, this );

    scan_installed_airports( fg_scenery );
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

    char buf[10];
    sprintf( buf, "%02d%c", heading, c );
    return string(buf);
}

void
AirportBrowser::show_runways( const apt_dat_t* apt )
{
    if (apt == 0)
	return;

    runways_->clear();
    runways_->add( "<default>" );
    for (unsigned int i = 0; i < apt->runways_.size(); ++i)
    {
	string rwy( apt->runways_[i] );
	runways_->add( rwy.c_str() );
	string rev = reverse_runway( rwy );
	if (!rev.empty())
	    runways_->add( rev.c_str() );
    }
    runways_->select( 1 );
}

void
AirportBrowser::browser_cb( Fl_Widget* o, void* v )
{
    ((AirportBrowser*)v)->browser_cb();
}

void
AirportBrowser::browser_cb()
{
    table_->browser_cb();
    show_runways( table_->get_selected() );
}

string
AirportBrowser::get_selected_id() const
{
    const apt_dat_t* apt = table_->get_selected();
    return apt == 0 ? string("") : apt->id_;
}

string
AirportBrowser::get_selected_name() const
{
    const apt_dat_t* apt = table_->get_selected();
    return apt == 0 ? string("") : apt->name_;
}

string
AirportBrowser::get_selected_runway() const
{
    int n = runways_->value();
    return n > 1 ? string( runways_->text(n) ) : string("");
}
