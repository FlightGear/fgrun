// fgrun_pty.hxx -- Portable pseudo-tty functions.
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

#ifndef FGRUN_PTY_H
#define FGRUN_PTY_H

struct termios;
struct winsize;

pid_t pty_fork( int* amaster,
		char* name,
		struct termios* termp,
		struct winsize* winp );

// int pty_open( int* amaster,
// 	      int* aslave,
// 	      char* name,
// 	      struct termios* termp,
// 	      struct winsize* winp );

// int tty_login( int fd );
	      
#endif // FGRUN_PTY_H
