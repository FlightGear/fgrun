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
#include <string>
#include <zlib.h>

#include <FL/Fl_Input.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Hold_Browser.H>

#include "apt_dat.h"

class AirportTable;

/**
 * An airport and runway browser.
 * Data is read from FlightGear's runways.dat.gz file.
 */
class AirportBrowser : public Fl_Group
{
public:

    AirportBrowser( int X, int Y, int W, int H, const char* l=0 );
    ~AirportBrowser();

    void init( const std::string& fg_root, const std::string& fg_scenery );

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

    /**
     * 
     */
    void select_id( const std::string& id );

    /**
     * 
     */
    void scan_installed_airports( const std::string& dir );

    /**
     * 
     */
    void show_installed();

    int handle( int e );

    /**
     * 
     */
    bool loaded() const { return loaded_; }

protected:

    void draw();

private:

    static void col_header_cb( Fl_Widget*, void* );
    static void id_cb( Fl_Widget*, void* );
    void id_cb();
    static void name_cb( Fl_Widget*, void* );
    void name_cb();

    const apt_dat_t* find( const std::string& id ) const;

    static void idle_proc( void* );
    void idle_proc();

    static void browser_cb( Fl_Widget*, void* );
    void browser_cb();

    void show_runways( const apt_dat_t* apt );

private:
    AirportTable* table_;
    Fl_Hold_Browser* runways_;
    Fl_Input* id_;
    Fl_Input* name_;
    gzFile gzf_;

    /**
     * Array of installed airport names.
     */
    std::vector< std::string > installed_airports_;

    /**
     * Array of all airports and runways loaded from runways.dat.
     */
    std::vector< apt_dat_t > airports_;

    /**
     * True once runways.dat has been loaded.
     */
    bool loaded_;
};

#endif // AirportBrowser_h_included
