// main.cxx -- FlightGear Launch Control
//
// Written by Bernie Bright, started Oct 2002.
//
// Copyright (C) 2002  Bernie Bright - bbright@bigpond.net.au
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <FL/Fl.h>

#if defined(WIN32)
#  include "FGRun_Win32.h"
#else
#  include "FGRun_Posix.h"
#endif

int
main( int argc, char* argv[] )
{
    UserInterface* ui;

#if defined(WIN32)
    ui = new FGRun_Win32;
#else
    ui = new FGRun_Posix;
#endif

    ui->show();
    return Fl::run();
}
