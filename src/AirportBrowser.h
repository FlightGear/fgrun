// AirportBrowser.h -- Airport and runway browser widget.
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

#ifndef AirportBrowser_h_included
#define AirportBrowser_h_included

#include <vector>
#include <set>
#include <map>
#include <deque>
#include <string>
#include <zlib.h>

#include <FL/Fl_Group.H>

#include <simgear/misc/sg_path.hxx>

#include "apt_dat.h"

class Fl_Input;
class Fl_Hold_Browser;
class Fl_Button;
class AirportTable;

/**
 * An airport and runway browser.
 * Runway data is read from FlightGear's apt.dat.gz file.
 * Airports are located by searching the scenery directory.
 */
class AirportBrowser : public Fl_Group
{
public:

    AirportBrowser( int X, int Y, int W, int H, const char* l=0 );
    ~AirportBrowser();

    /**
     * Start loading runways.dat file in the background.
     * Invoke callback when loading complete.
     */
    void load_runways( const std::string&, Fl_Callback* cb =0, void* v =0 );

    /**
     * Start scanning scenery directories for airport files in the background.
     * Invoke callback when scan complete.
     */
    void load_airports( const std::vector<std::string>& dirs,
			const SGPath& cache,
			Fl_Callback* cb =0, void* v =0);

    /**
     * 
     */
    Fl_Color col_header_color() const;
    void col_header_color( Fl_Color );

    /**
     * 
     */
    Fl_Font col_header_font() const;
    void col_header_font( Fl_Font );

    /**
     * 
     */
    uchar col_header_fontsize() const;
    void col_header_fontsize( uchar );

    /**
     * 
     */
    int selection() const;
    std::string get_selected_id() const;
    std::string get_selected_name() const;
    std::string get_selected_runway() const;
    std::string get_selected_parking() const;

    /**
     * 
     */
    void select_id( const std::string& id );

    /**
     * 
     */
    void select_rwy( const std::string& id );

    /**
     * 
     */
    void select_parking( const std::string& id );

    /**
     * 
     */
    void show_installed( bool refresh = false );

    int handle( int e );

    /**
     * 
     */
    bool loaded() const;

    void set_refresh_callback( Fl_Callback* cbp, void* v );

    const apt_dat_t* find( const std::string& id ) const;

protected:

    void draw();

private:

    static void col_header_cb( Fl_Widget*, void* );
    static void id_cb( Fl_Widget*, void* );
    void id_cb();
    static void name_cb( Fl_Widget*, void* );
    void name_cb();

    static void refresh_cb( Fl_Widget*, void* );
    void refresh_cb();


    static void runways_idle_proc( void* );
    void runways_idle_proc();

    static void airports_idle_proc( void* );
    void airports_idle_proc();

    static void browser_cb( Fl_Widget*, void* );
    void browser_cb();

    void show_runways( const apt_dat_t* apt );

    void try_load_parking( apt_dat_t &data );
    bool load_parking( const SGPath &path, apt_dat_t &data );
    void show_parking( const apt_dat_t* apt );

    static void cb_runways_(Fl_Widget* o, void* v);
    static void cb_parking_(Fl_Widget* o, void* v);
    void cb_runways(Fl_Widget* o);
    void cb_parking(Fl_Widget* o);

private:
    AirportTable* table_;
    Fl_Hold_Browser* runways_;
    Fl_Hold_Browser* parking_;
    Fl_Input* id_;
    Fl_Input* name_;
    Fl_Button* refresh_;
    gzFile gzf_;

    /**
     * Directory names to search while scanning
     * for installed airport files.
     */
    std::deque< std::string > airports_dirs;

    /**
     * Names of all airports located in the scenery directories.
     */
    std::set< std::string > installed_airports_;

    /**
     * Names of all individual scenery directories.
     */
    std::set< std::string > installed_dirs_;

    /**
     * Names of airports and runways loaded from runways.dat.
     */
    std::vector< apt_dat_t > airports_;
    std::map< std::string, std::vector< std::string > > airports_by_tiles_;

    /**
     * True once runways.dat has been loaded.
     */
    bool runways_loaded_;
    bool airports_loaded_;

    /**
     * Callback function to invoke after airport scan is complete.
     */
    Fl_Callback* airports_loaded_cb_;
    void* airports_loaded_cb_data_;

    /**
     * Callback function to invoke after runways.dat is loaded. 
     */
    Fl_Callback* runways_loaded_cb_;
    void* runways_loaded_cb_data_;

    /*
     * Installed airport name cache file.
     */
    SGPath airports_cache_;

    Fl_Callback* refresh_cb_;
    void* refresh_cb_data_;

    SGPath fg_root_;
};

#endif // AirportBrowser_h_included
