// AirportTable.h -- Airport table.
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

#ifndef AirportTable_h_included
#define AirportTable_h_included

#include <vector>

#include "Fl_Table_Row.H"

struct apt_dat_t;

/**
 * 
 */
class AirportTable : public Fl_Table_Row
{
public:

    typedef std::vector< const apt_dat_t* > rowdata_type;
    typedef rowdata_type::iterator iterator;

    AirportTable( int X, int Y, int W, int H, const char* l=0 );
    ~AirportTable();

    void browser_cb();
    void col_header_cb( int col );
    void cell_cb();

    const apt_dat_t* select_id( const char* id );
    const apt_dat_t* select_name( const char* name );
    const apt_dat_t* get_selected() const;

    void set_airports( const rowdata_type& apts );
    void resize( int X, int Y, int W, int H );

protected:

    void draw_cell( TableContext context, int R=0, int C=0,
		    int X=0, int Y=0, int W=0, int H=0 );

    void sort_column( int col, bool reverse = false );

private:
    rowdata_type rowdata_;
    bool sort_reverse_;
    int sort_lastcol_;
    int selected_;
};

#endif // AirportTable_h_included
