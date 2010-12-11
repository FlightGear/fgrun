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
#include <iterator>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>

#include <FL/filename.H>
#include <FL/Fl_Round_Button.H>

#include "AirportBrowser.h"
#include "AirportTable.h"
#include "parkingloader.h"
#include "i18n.h"

#include <FL/Fl_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Input.H>

using std::string;
using std::vector;
using std::deque;

AirportBrowser::AirportBrowser( int X, int Y, int W, int H,
				const char* l )
    : Fl_Group( X, Y, W, H, l )
    , gzf_(0)
    , runways_loaded_(false)
    , airports_loaded_(false)
    , refresh_cb_(0)
{
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

    Fl_Group *g = new Fl_Group( X, Y+th+5, X+75+5+tw-75, 25 );
    g->begin();
    id_ = new Fl_Input( X, Y+th+5, 75, 25 );
    id_->textsize( 12 );
    id_->callback( id_cb, this );
    id_->when( FL_WHEN_CHANGED );

    name_ = new Fl_Input( X+75+5, Y+th+5, tw-75-20, 25 );
    name_->textsize( 12 );
    name_->callback( name_cb, this );
    name_->when( FL_WHEN_CHANGED );
    g->end();
    g->resizable(name_);

    int rh = ( th - 30 ) / 2;
    runways_ = new Fl_Hold_Browser( X+tw+5, Y, 120, rh, _("Runways") );
    runways_->align( FL_ALIGN_TOP );
    runways_->callback( &AirportBrowser::cb_runways_ );

    {   Fl_Group *o = new Fl_Group( X+tw+5, Y+rh, 120, th-rh );
        parking_ = new Fl_Hold_Browser( X+tw+5, Y+rh+30, 120, th-rh-30, _("Parking") );
        parking_->align( FL_ALIGN_TOP );
        parking_->callback( &AirportBrowser::cb_parking_ );
        o->end();
        o->resizable( parking_ );
    }

    refresh_ = new Fl_Button( X+tw+5, Y+th+5, 120, 25, _("Refresh") );
    refresh_->labelsize(12);
    refresh_->callback( refresh_cb, this );
    refresh_->tooltip( _("Reload airports") );
    end();

    resizable( table_ );
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
    show_parking( apt );
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
AirportBrowser::runways_idle_proc( void* v )
{
    ((AirportBrowser*)v)->runways_idle_proc();
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
AirportBrowser::runways_idle_proc( )
{
    static const char space[] = " \t\n\r";
    static const char junk[] = "?\n\r";
    int count = 200;
    char line[512];
    char *token;
    string location;

    setlocale( LC_ALL, "C" );

    while (count-- > 0 && gzgets( gzf_, line, sizeof line ) != 0)
    {
        // Read 'count' airports, or until EOF or error.
        token = strtok(line, space);       

        if(token == NULL) {
            continue;
        } else if(!strcmp(token, "99")) {
            break;
        }

        int type_num = atoi(token);
        if ( type_num == 1 || type_num == 16 || type_num == 17 )  // It's an airport, an heliport or a seabase
        {
            location = "";

            token = strtok(NULL, space);
            token = strtok(NULL, space);
            token = strtok(NULL, space);
            token = strtok(NULL, space);

            // We've found the airport
            apt_dat_t apt;
            apt.id_ = token;
            apt.name_ = strtok(NULL, junk);
            apt.type_ = type_num;
            if (apt.name_[0] == ' ')
                apt.name_.erase( 0, 1);
            if (apt.name_.find( "[H] " ) == 0)
            {
                apt.name_.erase(0, 4);
                apt.name_ += " [H]";
            }
            else if (apt.name_.find( "[S] " ) == 0)
            {
                apt.name_.erase(0, 4);
                apt.name_ += " [S]";
            }

            try_load_parking( apt );

            airports_.push_back( apt );
        }
        else if (type_num == 10)	// Now read in the runways and taxiways
        {
            if ( location.empty() )
            {
                token = strtok(NULL, space);
                float lat = strtod( token, 0 );
                token = strtok(NULL, space);
                float lon = strtod( token, 0 );
                std::ostringstream os;
                os << ( lon < 0 ? 'w' : 'e' ) << std::setfill('0') << std::setw( 3 ) << (int)fabs( floor( lon ) ) << ( lat < 0 ? 's' : 'n' ) << std::setfill( '0' ) << std::setw( 2 ) << (int)fabs( floor( lat ) );
                location = os.str();

                apt_dat_t &apt = airports_.back();
                airports_by_tiles_[ location ].push_back( apt.id_ );
                apt.lon_ = lon;
                apt.lat_ = lat;
            }
            else
            {
                strtok(NULL, space);
                strtok(NULL, space);
            }
            string rwy( strtok(NULL, space) );
            if (rwy != "xxx")
            {
                if ( rwy.length() > 1 && rwy[ rwy.length() - 1 ] == 'x' )
                    rwy.erase( rwy.length() - 1 );
                airports_.back().runways_.push_back( rwy );
	        string rev = reverse_runway( rwy );
	        if (!rev.empty())
	            airports_.back().runways_.push_back( rev );
            }
        }
        else if (type_num == 100)	// Runways (850)
        {
            strtok(NULL, space);
            strtok(NULL, space);
            strtok(NULL, space);
            strtok(NULL, space);
            strtok(NULL, space);
            strtok(NULL, space);
            strtok(NULL, space);
            string rwy( strtok(NULL, space) );
            airports_.back().runways_.push_back( rwy );
            if ( location.empty() )
            {
                token = strtok(NULL, space);
                float lat = strtod( token, 0 );
                token = strtok(NULL, space);
                float lon = strtod( token, 0 );
                std::ostringstream os;
                os << ( lon < 0 ? 'w' : 'e' ) << std::setfill('0') << std::setw( 3 ) << (int)fabs( floor( lon ) ) << ( lat < 0 ? 's' : 'n' ) << std::setfill( '0' ) << std::setw( 2 ) << (int)fabs( floor( lat ) );
                location = os.str();

                apt_dat_t &apt = airports_.back();
                airports_by_tiles_[ location ].push_back( apt.id_ );
                apt.lon_ = lon;
                apt.lat_ = lat;
            }
            else
            {
                strtok(NULL, space);
                strtok(NULL, space);
            }
            strtok(NULL, space);
            strtok(NULL, space);
            strtok(NULL, space);
            strtok(NULL, space);
            strtok(NULL, space);
            strtok(NULL, space);
            rwy = strtok(NULL, space);
            airports_.back().runways_.push_back( rwy );
        }
        else if (type_num == 101)	// Water runways (850)
        {
            strtok(NULL, space);
            strtok(NULL, space);
            string rwy( strtok(NULL, space) );
            airports_.back().runways_.push_back( rwy );
            if ( location.empty() )
            {
                token = strtok(NULL, space);
                float lat = strtod( token, 0 );
                token = strtok(NULL, space);
                float lon = strtod( token, 0 );
                std::ostringstream os;
                os << ( lon < 0 ? 'w' : 'e' ) << std::setfill('0') << std::setw( 3 ) << (int)fabs( floor( lon ) ) << ( lat < 0 ? 's' : 'n' ) << std::setfill( '0' ) << std::setw( 2 ) << (int)fabs( floor( lat ) );
                location = os.str();

                apt_dat_t &apt = airports_.back();
                airports_by_tiles_[ location ].push_back( apt.id_ );
                apt.lon_ = lon;
                apt.lat_ = lat;
            }
            else
            {
                strtok(NULL, space);
                strtok(NULL, space);
            }
            rwy = strtok(NULL, space);
            airports_.back().runways_.push_back( rwy );
        }
        else if (type_num == 102)	// Helipads (850)
        {
            string rwy( strtok(NULL, space) );
            if ( rwy.length() > 1 && rwy[ rwy.length() - 1 ] == 'x' )
                rwy.erase( rwy.length() - 1 );
            airports_.back().runways_.push_back( rwy );
            if ( location.empty() )
            {
                token = strtok(NULL, space);
                float lat = strtod( token, 0 );
                token = strtok(NULL, space);
                float lon = strtod( token, 0 );
                std::ostringstream os;
                os << ( lon < 0 ? 'w' : 'e' ) << std::setfill('0') << std::setw( 3 ) << (int)fabs( floor( lon ) ) << ( lat < 0 ? 's' : 'n' ) << std::setfill( '0' ) << std::setw( 2 ) << (int)fabs( floor( lat ) );
                location = os.str();

                apt_dat_t &apt = airports_.back();
                airports_by_tiles_[ location ].push_back( apt.id_ );
                apt.lon_ = lon;
                apt.lat_ = lat;
            }
        }
    }

    if (gzeof( gzf_ ))
    {
	gzclose( gzf_ );
	Fl::remove_idle( runways_idle_proc, this );
	std::sort( airports_.begin(), airports_.end(), apt_id_comp );
	runways_loaded_ = true;
	if (runways_loaded_cb_ != 0)
	    runways_loaded_cb_( this, runways_loaded_cb_data_ );
    }

    setlocale( LC_ALL, "" );
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
AirportBrowser::show_installed( bool refresh )
{
    static vector<const apt_dat_t*> apts;
    typedef vector<string>::size_type size_type;

    if (refresh)
	apts.clear();

    if (apts.empty())
    {
	size_type count = installed_airports_.size();
	apts.reserve( count );
        for (std::set<std::string>::iterator i = installed_airports_.begin(); i != installed_airports_.end(); ++i)
	{
	    const apt_dat_t* apt = find( *i );
	    if (apt != 0)
	    {
		apts.push_back( apt );
	    }
	}
    }

    table_->set_airports( apts );
    id_->activate();
    name_->activate();
}

bool rwy_comp( const string& a, const string& b )
{
    return a.substr(0,2) < b.substr(0,2);
}

void
AirportBrowser::show_runways( const apt_dat_t* apt )
{
    if (apt == 0)
	return;

    vector< string > rwys;
    unsigned int i;

    for (i = 0; i < apt->runways_.size(); ++i)
    {
	rwys.push_back( apt->runways_[i] );
    }
    std::sort( rwys.begin(), rwys.end() );

    runways_->clear();
    runways_->add( _("<default>") );
    for (i = 0; i < rwys.size(); ++i)
    {
	runways_->add( rwys[i].c_str() );
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
    show_parking( table_->get_selected() );
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

string
AirportBrowser::get_selected_parking() const
{
    int n = parking_->value();
    return n > 0 ? string( parking_->text(n) ) : string("");
}

void
AirportBrowser::select_id( const string& id )
{
    const apt_dat_t* apt = table_->select_id( id.c_str() );
    show_runways( apt );
    show_parking( apt );
}

void
AirportBrowser::select_rwy( const string& id )
{
    for ( int i = 1; i < runways_->size(); ++i )
    {
        if ( id == runways_->text(i) )
        {
            runways_->select( i );
            parking_->deselect();
            break;
        }
    }
}

void
AirportBrowser::select_parking( const string& id )
{
    for ( int i = 1; i < parking_->size(); ++i )
    {
        if ( id == parking_->text(i) )
        {
            parking_->select( i );
            runways_->deselect();
            break;
        }
    }
}

void
AirportBrowser::load_runways( const string& fg_root, Fl_Callback* cb, void* v )
{
    this->fg_root_ = fg_root;

    runways_loaded_cb_ = cb;
    runways_loaded_cb_data_ = v;

    airports_.clear();
    airports_.reserve( 27000 );

    gzf_ = gzopen( ( fg_root + "/Airports/apt.dat.gz" ).c_str(), "rb" );
    if (gzf_ == 0)
    {
        throw _("gzopen error");
    }

    // Skip first line.
    char c;
    while ((c = gzgetc(gzf_)) != -1 && c != '\n')
        ;

    runways_loaded_ = false;
    airports_loaded_ = false;
    id_->deactivate();
    name_->deactivate();

    // Load the file in the background.
    Fl::add_idle( runways_idle_proc, this );
}

/**
 * 
 */
void
AirportBrowser::load_airports( const vector<string>& dirs,
			       const SGPath& cache,
			       Fl_Callback* cb, void* v )
{
    table_->clear();
    runways_->clear();
    installed_airports_.clear();
    installed_dirs_.clear();
    airports_loaded_cb_ = cb;
    airports_loaded_cb_data_ = v;
    airports_cache_ = cache;
    airports_loaded_ = false;

    if (cache.exists())
    {
        bool airports = true;
	std::ifstream ifs( cache.str().c_str() );
        while ( ifs.good() ) {
            string s;
            ifs >> s;
            if ( s == "--dirs--" ) {
                airports = false;
                continue;
            }
            if ( airports )
                installed_airports_.insert( s );
            else
                installed_dirs_.insert( s );
        }
	airports_loaded_ = true;
	if (airports_loaded_cb_ != 0)
	    airports_loaded_cb_( this, airports_loaded_cb_data_ );
    }
    else
    {
	airports_dirs.clear();
	std::copy( dirs.begin(), dirs.end(),
		   std::back_inserter( airports_dirs ) );

	airports_loaded_ = false;
	Fl::add_idle( airports_idle_proc, this );
    }
}

void
AirportBrowser::airports_idle_proc( void* v )
{
    ((AirportBrowser*)v)->airports_idle_proc();
}

/**
 * Scan FlightGear Scenery directory and sub-directories for airport files.
 * Airpots ICAO ids are saved in a cache file.
 */
void
AirportBrowser::airports_idle_proc()
{
    if (airports_dirs.empty())
    {
	Fl::remove_idle( airports_idle_proc, this );

        for (std::set<string>::iterator i = installed_dirs_.begin(); i != installed_dirs_.end(); ++i)
        {
            for (size_t j = 0; j < airports_by_tiles_[ *i ].size(); ++j)
            {
                string s = airports_by_tiles_[ *i ][ j ];
	        const apt_dat_t* apt = find( airports_by_tiles_[ *i ][ j ] );
	        if ( apt != 0 && ( apt->type_ == 16 || apt->type_ == 17 ) )
	        {
		    installed_airports_.insert( apt->id_ );
	        }
            }
        }

	// Save airports ids to the cache file.
	if (!airports_cache_.str().empty())
	{
	    std::ofstream ofs( airports_cache_.str().c_str() );
	    if (ofs)  // !ofs.fail()
	    {
		std::copy( installed_airports_.begin(),
			   installed_airports_.end(),
			   std::ostream_iterator<string>( ofs, "\n" ) );
                ofs << "--dirs--" << std::endl;
		std::copy( installed_dirs_.begin(),
			   installed_dirs_.end(),
			   std::ostream_iterator<string>( ofs, "\n" ) );
	    }
	}

	airports_loaded_ = true;
	if (airports_loaded_cb_ != 0)
	    airports_loaded_cb_( this, airports_loaded_cb_data_ );

	return;
    }

    string cwd( airports_dirs.front() );

    // mingw requires a trailing slash on directory names.
    if (cwd[ cwd.length() - 1 ] != '/')
	cwd.append( "/" );

    dirent** files;
    int n = fl_filename_list( cwd.c_str(), &files, fl_numericsort );
    if (n > 0)
    {
        bool dir_added = false;
	for (int i = 0; i < n; ++i)
	{
	    if (fl_filename_match( files[i]->d_name,
				   "[ew][01][0-9][0-9][ns][0-9][0-9]") ||
		fl_filename_match( files[i]->d_name,
				   "[ew][01][0-9][0-9][ns][0-9][0-9]/"))
	    {
		// Found a scenery sub-directory.
		string d(cwd);
		d.append( files[i]->d_name );
		if (fl_filename_isdir( d.c_str() ) )
		{
		    airports_dirs.push_back( d );
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
		installed_airports_.insert( string( files[i]->d_name ) );

                if ( !dir_added )
                {
                    string d( cwd );
                    if (d[ d.length() - 1 ] == '/')
	                d.erase( d.length() - 1 );
                    string::size_type p = d.rfind( "/" );
                    if ( p != string::npos )
                        installed_dirs_.insert( d.substr( p + 1 ) );
                    dir_added = true;
                }
	    }

	    free( files[i] );
	}

	free( files );
    }

    airports_dirs.pop_front();
}

void
AirportBrowser::refresh_cb( Fl_Widget* o, void* v )
{
    static_cast<AirportBrowser*>(v)->refresh_cb();
}

void
AirportBrowser::refresh_cb()
{
    if (refresh_cb_)
	refresh_cb_( this, refresh_cb_data_ );
}

void
AirportBrowser::set_refresh_callback( Fl_Callback* cbp, void* v )
{
    refresh_cb_ = cbp;
    refresh_cb_data_ = v;
}

bool
AirportBrowser::loaded() const
{
    return runways_loaded_ && airports_loaded_;
}

void
AirportBrowser::try_load_parking( apt_dat_t &apt )
{
    SGPath path = fg_root_;
    path.append( "AI/Airports" );
    path.append( apt.id_ );
    path.append( "parking.xml" );
    load_parking( path, apt );
}

bool
AirportBrowser::load_parking( const SGPath &path, apt_dat_t &data )
{
    ParkingLoader visitor( data );
    if ( path.exists() )
    {
        try {
            readXML( path.str(), visitor );
            return true;
        } catch (const sg_exception &) {
            //cerr << "unable to read " << parkpath.str() << endl;
        }
    }
    return false;
}

void
AirportBrowser::show_parking( const apt_dat_t* apt )
{
    if (apt == 0 || apt->id_.empty())
	return;

    parking_->clear();
    for ( std::set<std::string>::const_iterator ii = apt->parking_.begin(); ii != apt->parking_.end(); ++ii )
    {
	parking_->add( ii->c_str() );
    }
}

void
AirportBrowser::cb_runways_(Fl_Widget* o, void* v)
{
    static_cast<AirportBrowser*>( o->parent() )->cb_runways(o);
}

void
AirportBrowser::cb_parking_(Fl_Widget* o, void* v)
{
    static_cast<AirportBrowser*>( o->parent()->parent() )->cb_parking(o);
}

void
AirportBrowser::cb_runways(Fl_Widget* o)
{
    parking_->deselect();
}

void
AirportBrowser::cb_parking(Fl_Widget* o)
{
    runways_->deselect();
}
