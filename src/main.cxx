// main.cxx -- FlightGear Launch Control
//
// Written by Bernie Bright, started Oct 2002.
//
// Copyright (c) 2002  Bernie Bright - bbright@users.sourceforge.net
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

#include <string>
#include <iostream>
#include <FL/Fl.h>

#if defined(WIN32)
#  include "FGRun_Win32.h"
#else
#  include "FGRun_Posix.h"
#endif

static std::string def_fg_exe = "";
static std::string def_fg_root = "";
static std::string def_fg_scenery = "";

/**
 * --fg-exe=<PATH>
 * --fg-root=<DIR>
 * --fg-scenery=<DIR>
 */
static int
parse_arg( int, char** argv, int& i )
{
    if (strncmp( argv[i], "--fg-exe=", 9 ) == 0)
    {
	def_fg_exe.assign( &argv[i][9] );
	++i;
	return 1;
    }

    if (strncmp( argv[i], "--fg-root=", 10 ) == 0)
    {
	def_fg_root.assign( &argv[i][10] );
	def_fg_scenery = def_fg_root;
	def_fg_scenery += "/Scenery";

	++i;
	return 1;
    }

    if (strncmp( argv[i], "--fg-scenery=", 13 ) == 0)
    {
	def_fg_scenery.assign( &argv[i][13] );
	++i;
	return 1;
    }

//     if (strcmp( argv[i], "--advanced" ) == 0)
//     {
//     }
//     if (strcmp( argv[i], "--wizard" ) == 0)
//     {
//     }

    return 0;
}

int
main( int argc, char* argv[] )
{
    int i = 0;
    if (Fl::args( argc, argv, i, parse_arg ) < argc)
    {
	Fl::fatal("Options are:\n --fg-exe=<PATH>\n --fg-root=<DIR>\n --fg-scenery=<DIR>\n%s", Fl::help );
    }

//     std::cout << "def_fg_exe='" << def_fg_exe << "'\n";
//     std::cout << "def_fg_root='" << def_fg_root << "'\n";
//     std::cout << "def_fg_scenery='" << def_fg_scenery << "'\n";

    UserInterface* ui;

#if defined(WIN32)
    ui = new FGRun_Win32;
#else
    ui = new FGRun_Posix;
#endif

    ui->show();
    return Fl::run();
}
