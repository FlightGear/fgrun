// Fl_Heading_Dial.cxx -- Custom Fl_Dial with wrap around.
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

#include <FL/Fl.H>
#include <FL/math.h>
#include <FL/fl_draw.H>

#include "Fl_Heading_Dial.H"

Fl_Heading_Dial::Fl_Heading_Dial( int x, int y, int w, int h, const char *l )
    : Fl_Dial(x,y,w,h,l)
{
    type(1);
    angles( -180, 180 );
    maximum( 360 );
}

int
Fl_Heading_Dial::handle( int event )
{
    int X = x();
    int Y = y();
    int W = w();
    int H = h();

    if (event == FL_DRAG)
    {
	short a1 = angle1();
	short a2 = angle2();

	int mx = (Fl::event_x() - X - W / 2) * H;
	int my = (Fl::event_y() - Y - H / 2) * W;
	if (!mx && !my) return 1;
	double angle = 270 - atan2((float)-my, (float)mx) * 180 / M_PI;
	double oldangle = (a2 - a1) * (value() - minimum()) / (maximum()-minimum()) + a1;
	while (angle < oldangle-180) angle += 360;
	while (angle > oldangle+180) angle -= 360;

	// Calculate new value with wrap.
	double val = minimum() + (maximum()-minimum())*(angle-a1)/(a2-a1);
	while (val < 0) val += 360;
	while (val >= 360) val -= 360;

	handle_drag(clamp(round(val)));
	return 1;
    }
    else
    {
	return Fl_Dial::handle( event, X, Y, W, H );
    }
}

void
Fl_Heading_Dial::draw()
{
    Fl_Dial::draw();
    draw_ticks();
}

void
Fl_Heading_Dial::draw_ticks()
{
    int ox = x();
    int oy = y();
    int ww = w();
    int hh = h();
    int side = ww;
    oy = oy + (hh-side) / 2;

    double step = 30 * M_PI / 180.0;
    double x1, y1, x2, y2;
    double cx = ox + side / 2;
    double cy = oy + side / 2;
    double rds = side / 2;

    for (int i = 0; i < 12; ++i)
    {
	double na = i * step;
	double ca = cos( na );
	double sa = sin( na );
	x1 = cx + rds * ca;
	y1 = cy - rds * sa;
	x2 = cx + (rds-6) * ca;
	y2 = cy - (rds-6)*sa;
	fl_color( FL_BLACK );
	fl_line( int(x1), int(y1), int(x2), int(y2) );
	fl_color( FL_WHITE );
    }
}
