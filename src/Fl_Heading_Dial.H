// Fl_Heading_Dial.H -- Custom Fl_Dial with wrap around.
//
// Written by Bernie Bright, started Dec 2003.
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

#ifndef Fl_Heading_Dial_H
#define Fl_Heading_Dial_H

#include <FL/Fl_Dial.H>

/**
 * Like Fl_Dial but with wrap around and a fixed range of [0-360].
 */
class Fl_Heading_Dial : public Fl_Dial
{
public:

    Fl_Heading_Dial(int x,int y,int w,int h, const char *l = 0);
    int handle( int );

protected:
    void draw();
    void draw_ticks();
};

#endif // Fl_Heading_Dial_H
