// AirportBrowser.cxx -- Fl_Table based airport browser.
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

#include <iostream>
#include <algorithm>
#include <string>
#include <cctype>

#include <FL/fl_draw.h>

#include "AirportBrowser.h"
#include "airportdb.h"
#include "UserInterface.h"

using std::string;

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
	    return reverse_ ? (b->name_ < a->name_) : (a->name_ < b->name_);
    }
};

AirportBrowser::AirportBrowser( int x, int y, int w, int h, const char *l )
    : Fl_Table_Row(x,y,w,h,l)
    , sort_reverse_(false)
    , sort_lastcol_(0)
    , selected_(-1)
{
}

AirportBrowser::~AirportBrowser()
{
}

void
AirportBrowser::set_airports( const airports_t& apts )
{
    clear();
    fl_cursor( FL_CURSOR_WAIT );
    Fl::check();

    rowdata_ = apts;
    rows( int(apts.size()) );
    cols(2);
    col_header(1);
    //col_resize(1);
    col_width( 1, tiw-col_width(0) );
    row_height_all( 16 );
    fl_cursor( FL_CURSOR_DEFAULT );
}

void
AirportBrowser::sort_column( int col, bool reverse )
{
    std::sort( rowdata_.begin(), rowdata_.end(),
	       SortColumn( col, reverse ) );
    redraw();
}

void
AirportBrowser::draw_cell( TableContext context,
			   int R, int C, int X, int Y, int W, int H )
{
    switch (context)
    {
    case CONTEXT_STARTPAGE:
// 	fl_font( FL_COURIER, 14 );
	return;

    case CONTEXT_ROW_HEADER:
	fl_color( FL_RED );
	fl_rectf( X, Y, W, H );
	return;

    case CONTEXT_COL_HEADER:
	fl_font( FL_HELVETICA /*| FL_BOLD*/, 12 );
	fl_push_clip( X, Y, W, H );
	{
	    static char* headers[2] = { "ICAO Id", "Name" };
	    fl_draw_box( FL_THIN_UP_BOX, X, Y, W, H, col_header_color() );
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

	    const string& s = (C == 0 ? rowdata_[R]->id_ : rowdata_[R]->name_);
	    fl_color(FL_BLACK);
	    fl_draw(s.c_str(), X+2, Y, W, H, FL_ALIGN_LEFT);

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
AirportBrowser::col_header_cb( int C )
{
    if (sort_lastcol_ == C)
	sort_reverse_ = !sort_reverse_;
    else
	sort_reverse_ = false;

    sort_column( C, sort_reverse_ );
    sort_lastcol_ = C;
}

void
UserInterface::apt_browser_cb()
{
    switch( apt_browser->callback_context() )
    {
    case Fl_Table::CONTEXT_COL_HEADER:
	if (Fl::event() == FL_RELEASE && Fl::event_button() == 1)
	{
	    apt_browser->col_header_cb( apt_browser->callback_col() );
	}
	break;

    case Fl_Table::CONTEXT_CELL:
	if (Fl::event() == FL_RELEASE && Fl::event_button() == 1)
	{
	    apt_browser->cell_cb();
	    set_choice( airport, apt_browser->get_selected_id().c_str() );
	}
	break;

    default:
	return;
    }
}

void
AirportBrowser::cell_cb()
{
    selected_ = callback_row();
}

void
AirportBrowser::select_id( const char* id )
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
    }
}

class NoCaseCompare
{
private:
    static bool nocase_compare( char c1, char c2 )
    {
	return toupper(c1) < toupper(c2);
    }

public:

    NoCaseCompare() {}

    bool operator()( const apt_dat_t* a, const apt_dat_t* b ) const
    {
	return std::lexicographical_compare( a->name_.begin(), a->name_.end(),
					     b->name_.begin(), b->name_.end(),
					     nocase_compare );
    }
};

void
AirportBrowser::select_name( const char* name )
{
    if (sort_lastcol_ != 1)
    {
	sort_column( 1, sort_reverse_ );
	sort_lastcol_ = 1;
    }

    apt_dat_t key;
    key.name_ = string(name);

    select_all_rows( 0 ); // de-select all rows

    iterator i = std::lower_bound( rowdata_.begin(), rowdata_.end(),
				   &key, NoCaseCompare() );
    if (i != rowdata_.end())
    {
	selected_ = std::distance( rowdata_.begin(), i );
	if (sort_reverse_ && selected_ > 0)
	    --selected_;
	top_row( selected_ );
	select_row( selected_, 1 );
    }
}

string
AirportBrowser::get_selected_id() const
{
    return selected_ >= 0 ? rowdata_[selected_]->id_ : string("");
}
