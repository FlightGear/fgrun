// FGRunUI_Posix.cxx -- 
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

#include <string>
#include <cstdio>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#include <FL/Fl.h>

#include "FGRunUI_Posix.h"

using std::string;

/**
 * 
 */
void
FGRunUI_Posix::run_fgfs()
{
    this->write_dot_fgfsrc();

    //Create pipes for stdout and stderr.
    int fd1[2];
    int fd2[2];
    if (pipe( fd1 ) < 0 || pipe( fd2 ) < 0)
    {
	perror( "pipe error" );
	return;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
	perror( "fork error" );
	return;
    }
    else if (pid > 0)
    {
	// parent

	// Close write-end of pipes.
 	close( fd1[1] );
 	close( fd2[1] );

	// Establish callbacks to read from child's stdout/stderr.
	Fl::add_fd( fd1[0], FL_READ, stdout_cb, (void*)this );
	Fl::add_fd( fd2[0], FL_READ, stderr_cb, (void*)this );

	// Erase any previous output from display window.
	output_text->buffer()->remove(
			       0, output_text->buffer()->length() );
	output_window->show();
    }
    else
    {
	// child

	// Close the read end of pipes.
	close( fd1[0] );
	close( fd2[0] );

	// Redirect stdout to write end of pipe 1.
	close(1);
	if (dup( fd1[1] ) < 0)
	    perror( "dup(fd1[1] error");

	// Redirect stderr to write end of pipe 2.
	close(2);
	if (dup( fd2[1] ) < 0)
	    perror( "dup(fd2[1] error");

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

	execl( path.c_str(), arg0.c_str(), NULL );
    }
}

void
FGRunUI_Posix::stdout_cb( int fd, void* p )
{
    if (p != 0)
	((FGRunUI_Posix*)p)->stdout_cb_i( fd );
}

void
FGRunUI_Posix::stderr_cb( int fd, void* p )
{
    if (p != 0)
	((FGRunUI_Posix*)p)->stderr_cb_i( fd );
}


void
FGRunUI_Posix::stdout_cb_i( int fd )
{
    char buf[200];
    int r = read( fd, buf, sizeof(buf)-1 );
    if (r > 0)
    {
	buf[r] = 0;
	output_text->insert( buf );
	output_text->show_insert_position();
    }
    else
    {
	Fl::remove_fd( fd );
	close( fd );
	int status;
	waitpid( -1, &status, WNOHANG );
	return;
    }
}

void
FGRunUI_Posix::stderr_cb_i( int fd )
{
    char buf[200];
    int r = read( fd, buf, sizeof(buf)-1 );
    if (r > 0)
    {
	buf[r] = 0;
	output_text->insert( buf );
	output_text->show_insert_position();
    }
    else
    {
	Fl::remove_fd( fd );
	close( fd );
	int status;
	waitpid( -1, &status, WNOHANG );
	return;
    }
}
