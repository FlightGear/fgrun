// logwin.h -- FlightGear ouput display window.
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

#ifndef logwin_h_included
#define logwin_h_included

class Fl_Window;
class Fl_Text_Display;
class Fl_Menu_Item;

/**
 * 
 */
class LogWindow
{
public:
    LogWindow( int w, int h, const char* l );
    ~LogWindow();

    void show();
    void hide();

    void append( const char* line );
    void clear();

private:
    static void save_cb( Fl_Widget*, void* );
    void save_cb();

    static void close_cb( Fl_Widget*, void* );
    void close_cb();

private:
    Fl_Window* win;
    Fl_Text_Display* text;

    static Fl_Menu_Item menu_[];
};

#endif // logwin_h_included
