// output.cxx -- FlightGear ouput display window.
//
// Written by Bernie Bright, started Sep 2003.
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

#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>

#include "logwin.h"

Fl_Menu_Item LogWindow::menu_[] = {
    { "&File", 0, 0, 0,  64, 0, 0, 12, 56 },
    { "&Save",  FL_CTRL+'s', LogWindow::save_cb, 0, 128, 0, 0, 12, 56 },
    { "&Close", FL_CTRL+'w', LogWindow::close_cb, 0, 0, 0, 0, 12, 56 },
    {0},
    {0}
};

LogWindow::LogWindow( int w, int h, const char* l )
{
    win = new Fl_Window( w, h, l );
    {
	win->labelsize(12);
	win->user_data( this );
	{
	    Fl_Menu_Bar* o = new Fl_Menu_Bar( 0, 0, w, 25 );
	    o->labelsize(12);
	    o->textsize(12);
	    o->menu( menu_ );
	}

	text = new Fl_Text_Display( 0, 25, w, h-25 );
	text->buffer( new Fl_Text_Buffer );
	win->end();
	win->resizable( text );
    }
}

LogWindow::~LogWindow()
{
    delete win;
}

void
LogWindow::show()
{
    win->show();
}

void
LogWindow::hide()
{
    win->hide();
}

void
LogWindow::append( const char* line )
{
    text->insert_position( text->buffer()->length() );
    text->insert( line );
    text->show_insert_position();
}

void
LogWindow::clear()
{
    text->buffer()->remove( 0, text->buffer()->length() );
}

void
LogWindow::save_cb( Fl_Widget* o, void* v )
{
    ((LogWindow*)(o->parent()->user_data()))->save_cb();
}

void
LogWindow::save_cb()
{
    char* fname = fl_file_chooser( "Save file as?", "*.txt", 0, 0 );
    if (fname != 0)
    {
	text->buffer()->savefile( fname );
    }
}

void
LogWindow::close_cb( Fl_Widget* o, void* v )
{
    ((LogWindow*)(o->parent()->user_data()))->close_cb();
}

void
LogWindow::close_cb()
{
    win->hide();
}
