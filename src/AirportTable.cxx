// AirportTable.cxx -- Airport table.
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

#include <cctype>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/algorithm/string/case_conv.hpp>

#include <FL/fl_draw.H>

#include "AirportTable.h"
#include "apt_dat.h"
#include "i18n.h"

using std::string;
using std::vector;

/**
 * 
 */
class SortColumn
{
private:
    int col_;
    bool reverse_;

public:
    SortColumn( int col, bool reverse )
        : col_(col), reverse_(reverse) {}

    bool operator()( const apt_dat_t* a, const apt_dat_t* b ) const
    {
        if (col_ == 0)
            return reverse_ ? (b->id_ < a->id_) : (a->id_ < b->id_);
        else
            return reverse_ ?
		(boost::algorithm::to_lower_copy(b->name_) < boost::algorithm::to_lower_copy(a->name_)) :
		(boost::algorithm::to_lower_copy(a->name_) < boost::algorithm::to_lower_copy(b->name_));
    }
};

/**
 * Helper class to perform a case insensitve search
 * on airport names.
 */
class NoCaseFind
{
private:
    std::string key;
   
public:
    NoCaseFind( const char *name) : key( name) {}

    bool operator()( const apt_dat_t* a) const
    {
        std::string name( a->name_);
        std::string::size_type mini = std::min( name.size(), key.size() );
        for ( std::string::size_type i=0; i < mini; i++ ) {
	    if ( toupper( key[i]) != toupper(name[i]) )
	        return false;
        }
        return true;
   }
};

AirportTable::AirportTable( int X, int Y, int W, int H, const char* l )
    : Fl_Table_Row( X, Y, W, H, l )
    , sort_reverse_(false)
    , sort_lastcol_(0)
    , selected_(-1)
{
    cols(2);
    rows( 100 );
    col_header(1);
    col_width( 1, tiw - col_width(0) );
    row_height_all( 16 );
}

AirportTable::~AirportTable()
{
}

void
AirportTable::draw_cell( TableContext context,
			 int R, int C, int X, int Y, int W, int H )
{
    switch (context)
    {
    case CONTEXT_STARTPAGE:
//      fl_font( FL_COURIER, 14 );
        return;

    case CONTEXT_ROW_HEADER:
        fl_color( FL_RED );
        fl_rectf( X, Y, W, H );
        return;

    case CONTEXT_COL_HEADER:
        fl_font( FL_HELVETICA /*| FL_BOLD*/, 12 );
        fl_push_clip( X, Y, W, H );
        {
            static char* headers[2] = { _("ICAO Id"), _("Name") };
            fl_draw_box( FL_THIN_UP_BOX, X, Y, W, H,
			 col_header_color() );
            fl_color(FL_BLACK);
            fl_draw( headers[C], X+2, Y, W, H, FL_ALIGN_LEFT, 0, 0);

            if (C == sort_lastcol_)
            {
                int xlft = X+(W-6)-8,
                    xctr = X+(W-6)-4,
                    xrit = X+(W-6)-0,
                    ytop = Y+(H/2)-4,
                    ybot = Y+(H/2)+4;

                if ( sort_reverse_ )
                {
                    // ENGRAVED UP ARROW
                    fl_color(FL_WHITE);
                    fl_line(xrit, ybot, xctr, ytop);
                    fl_line(xrit, ybot, xlft, ybot);
                    fl_color(41);       // dark gray
                    fl_line(xlft, ybot, xctr, ytop);
                }
                else
                {
                    // ENGRAVED DOWN ARROW
                    fl_color(FL_WHITE);
                    fl_line(xrit, ytop, xctr, ybot);
                    fl_color(41);       // dark gray
                    fl_line(xlft, ytop, xrit, ytop);
                    fl_line(xlft, ytop, xctr, ybot);
                }
            }
 
        }
        fl_pop_clip();
        return;

    case CONTEXT_CELL:
        fl_font( FL_HELVETICA, 12 );
        fl_push_clip( X, Y, W, H );
        {
            fl_color( row_selected(R) ? selection_color() : FL_WHITE);
            fl_rectf(X, Y, W, H);

	    if (!rowdata_.empty())
	    {
		const string& s = (C == 0 ? rowdata_[R]->id_ : rowdata_[R]->name_);
		fl_color(FL_BLACK);
		fl_draw( s.c_str(), X+2, Y, W, H, FL_ALIGN_LEFT );
	    }

            // BORDER
            fl_color(FL_LIGHT2); 
            fl_rect(X, Y, W, H);
        }
        fl_pop_clip();
        return;

    case CONTEXT_ENDPAGE:
    case CONTEXT_RC_RESIZE:
    case CONTEXT_NONE:
    case CONTEXT_TABLE:
        return;
    }
}

void
AirportTable::browser_cb()
{
    switch( callback_context() )
    {
    case CONTEXT_COL_HEADER:
	if (Fl::event() == FL_RELEASE && Fl::event_button() == 1)
	{
	    col_header_cb( callback_col() );
	}
	break;

    case CONTEXT_CELL:
	if (Fl::event() == FL_RELEASE && Fl::event_button() == 1)
	{
	    selected_ = callback_row();
	    //cell_cb();
	}
	break;

    default:
	return;
    }
}

void
AirportTable::col_header_cb( int C )
{
    if (sort_lastcol_ == C)
        sort_reverse_ = !sort_reverse_;
    else
        sort_reverse_ = false;

    sort_column( C, sort_reverse_ );
    sort_lastcol_ = C;
}

void
AirportTable::cell_cb()
{
}

void
AirportTable::sort_column( int col, bool reverse )
{
    std::sort( rowdata_.begin(), rowdata_.end(),
               SortColumn( col, reverse ) );
    redraw();
}

const apt_dat_t*
AirportTable::select_id( const char* id )
{
    // Ensure we use the correct sort order.
    if (sort_lastcol_ != 0)
    {
        sort_column( 0, sort_reverse_ );
        sort_lastcol_ = 0;
    }

    apt_dat_t key;
    key.id_ = string(id);

    select_all_rows( 0 ); // de-select all rows

    iterator i = std::lower_bound( rowdata_.begin(), rowdata_.end(),
                           &key, SortColumn( 0, sort_reverse_ ) );
    if (i != rowdata_.end())
    {
        selected_ = std::distance( rowdata_.begin(), i );
        if (sort_reverse_ && selected_ > 0)
            --selected_;
        top_row( selected_ );
        select_row( selected_, 1 );
	return *i;
    }
    else
    {
	return 0;
    }
}

const apt_dat_t*
AirportTable::select_name( const char* name )
{
    // Ensure we use the correct sort order.
    if (sort_lastcol_ != 1)
    {
        sort_column( 1, sort_reverse_ );
        sort_lastcol_ = 1;
    }

   

    select_all_rows( 0 ); // de-select all rows
    
   iterator i = std::find_if( rowdata_.begin(), rowdata_.end(),  NoCaseFind( name ) );

    if (i != rowdata_.end())
    {
        selected_ = std::distance( rowdata_.begin(), i );
        if (sort_reverse_ && selected_ > 0)
            --selected_;
        top_row( selected_ );
        select_row( selected_, 1 );
	return *i;
    }
    else
    {
	return 0;
    }
}

void
AirportTable::set_airports( const vector<const apt_dat_t*>& apts )
{
    clear();
    fl_cursor( FL_CURSOR_WAIT ); Fl::flush();
    rowdata_ = apts;
    rows( int( apts.size() ) );
    cols(2);
    col_header(1);
    col_width( 1, tiw - col_width(0) );
    row_height_all( 16 );
    fl_cursor( FL_CURSOR_DEFAULT );
}

const apt_dat_t*
AirportTable::get_selected() const
{
    if (selected_ >= 0 && selected_ < (int)rowdata_.size())
	return rowdata_[ selected_ ];
    else
	return 0;
}

void
AirportTable::resize( int X, int Y, int W, int H )
{
    Fl_Table::resize( X, Y, W, H );
    col_width( 1, tiw - col_width(0) );
}
