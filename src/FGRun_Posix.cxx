// FGRun_Posix.cxx -- 
//
// Written by Bernie Bright, started Oct 2002.
//
// Copyright (c) 2002  Bernie Bright - bbright@bigpond.net.au
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

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#ifdef HAVE_PTY_H
#  include <pty.h>
#endif

#ifdef HAVE_TERMIOS_H
#  include <termios.h>
#endif

#include <iostream>
#include <string>
#include <FL/Fl_Text_Display.H>

#include "FGRun_Posix.h"

using std::string;
using std::cout;

FGRun_Posix::FGRun_Posix()
    : win(0)
    , view(0)
{
}

FGRun_Posix::~FGRun_Posix()
{
    delete win;
}

static Fl_Menu_Item menu_[] = {
    { "&File", 0, 0, 0, 64, 0, 0, 12, 56 },
    { "&Save", 0x40073, 0, 0, 128, 0, 0, 12, 56 },
    { "&Close", 0, 0, 0, 0, 0, 0, 12, 56 },
    {0},
    {0}
};

void
FGRun_Posix::create_output_window()
{
    win = new Fl_Window( 640, 480, "FlightGear output" );
    {
	win->labelsize(12);
	win->user_data( this );
	{
	    Fl_Menu_Bar* o = new Fl_Menu_Bar( 0, 0, 640, 25 );
	    o->labelsize(12);
	    o->textsize(12);
	    o->menu( ::menu_ );
	}
	view = new Fl_Text_Display( 0, 25, 640, 480 );
	view->buffer( new Fl_Text_Buffer );
	win->resizable( win );
    }
}

void
FGRun_Posix::run_fgfs_impl()
{
#if defined(HAVE_TERMIOS_H)
    struct termios term;
    tcgetattr( STDOUT_FILENO, &term );
    term.c_oflag &= ~( OLCUC | ONLCR );
#endif

    int master = -1;

#if defined(HAVE_PTY_H)
    pid_t pid = forkpty( &master, 0, &term, 0 );
#else
    pid_t pid = fork();
#endif

    if (pid < 0)
    {
	perror( "fork error" );
	close( master );
	return;
    }

    if (pid > 0)
    {
	// parent
// 	if (output_to_window->value())
	{
	    if (win == 0)
	    {
		create_output_window();
	    }
	    else
	    {
		view->buffer()->remove( 0, view->buffer()->length() );
	    }

	    win->show();

	    Fl::add_fd( master, stdout_cb, this );
	}
	return;
    }
    else
    {
	// child
	close( master );

	string path = fg_exe->value();
	string arg0;
	string::size_type idx = path.find_last_of( "/\\" );
	if (idx != string::npos)
	{
	    arg0 = path.substr( idx+1 );
	}
	else
	{
	    arg0 = path;
	}

	// Append any new vars to environ array.
	int n = env_list->size();
	for (int i = 1; i <= n; ++i )
	{
	    // Need to keep this string value around.
	    char* s = strdup( env_list->text( i ) );
	    putenv( s );
	}
	execl( path.c_str(), arg0.c_str(), NULL );
    }
}

void
FGRun_Posix::stdout_cb( int fd, void* p )
{
    static_cast<FGRun_Posix*>(p)->stdout_cb( fd );
}

void
FGRun_Posix::stdout_cb( int fd )
{
    char buf[256];
    ssize_t n = read( fd, buf, sizeof( buf ) - 1 );
    if (n > 0)
    {
	buf[n] = 0;
	// Ensure new text is added at the end.
	view->insert_position( view->buffer()->length() );
	view->insert( buf );
	view->show_insert_position();
    }
    else
    {
	Fl::remove_fd( fd );
	close( fd );

	int status;
	int pid = wait( &status );
    }
}
