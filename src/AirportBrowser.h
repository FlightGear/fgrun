// AirportBrowser.h -- Fl_Table based airport browser.
//
// Written by Bernie Bright, started Aug 2003.
//
// Copyright (c) 2003  Bernie Bright - bbright@bigpond.net.au
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

#ifndef AIRPORT_BROWSER_H_
#define AIRPORT_BROWSER_H_

#include <vector>
#include <string>

#include "Fl_Table_Row.H"

struct apt_dat_t;

/**
 * 
 */
class AirportBrowser : public Fl_Table_Row
{
public:
    typedef std::vector< const apt_dat_t* > airports_t;
    typedef airports_t::iterator iterator;
    typedef airports_t::const_iterator const_iterator;
    typedef airports_t::reverse_iterator reverse_iterator;

public:
    AirportBrowser( int x, int y, int w, int h, const char *l=0 );
    ~AirportBrowser();

    void set_airports( const airports_t& apts );

    void select_id( const char* id );
    void select_name( const char* name );

    std::string get_selected_id() const;

    void col_header_cb( int col );
    void cell_cb();

protected:
    void draw_cell( TableContext context, int R=0, int C=0,
		    int X=0, int Y=0, int W=0, int H=0 );
    void sort_column( int col, bool reverse = false );

private:
    airports_t rowdata_;
    bool sort_reverse_;
    int sort_lastcol_;
    int selected_;
};

#endif // AIRPORT_BROWSER_H_
