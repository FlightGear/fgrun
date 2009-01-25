// FGRun_Posix.cxx -- 
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

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#ifdef HAVE_TERMIOS_H
#  include <termios.h>
#endif

#include <string>
#include <FL/filename.H>

#include "wizard.h"
#include "fgrun_pty.h"
#include "logwin.h"

using std::string;

int
Wizard::run_fgfs( const std::string &args )
{
    pid_t pid;
    int master = -1;

#if defined(HAVE_TERMIOS_H)
    struct termios term;
    tcgetattr( STDOUT_FILENO, &term );
    term.c_oflag &= ~( OLCUC | ONLCR );

    pid = pty_fork( &master, 0, &term, 0 );
#else
    pid = pty_fork( &master, 0, 0, 0 );
#endif

    if (pid < 0)
    {
	perror( "fork error" );
	(void) close( master );
	return 0;
    }

    if (pid > 0)
    {
	// parent

	if (master < 0)
	    return 0;

	Fl::lock();
	logwin->clear();
	logwin->show();

	Fl::add_fd( master, stdout_cb, this );
	Fl::unlock();

	int status;
	waitpid( pid, &status, 0 );

	return 0;
    }
    else
    {
	// child

	if (master >= 0)
	    close( master );

	const int buflen = FL_PATH_MAX;
	char buf[ buflen ];
	prefs.get( "fg_exe", buf, "", buflen-1);

	string path = buf;
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

	// "export" any environment variables.
	int iVal;
	prefs.get( "env-count", iVal, 0 );
	for (int i = 1; i <= iVal; ++i)
	{
	    buf[0] = 0;
	    prefs.get( Fl_Preferences::Name("env-var-%d", i),
		       buf, "", buflen-1 );
	    char* s = strdup( buf );
	    putenv( s );
	}
	vector<string> argv;
	argv.push_back( arg0 );
	string token;
	string line = args;
	while ( line.size() )
	{
	    idx = line.find( ' ' );
	    if ( idx == string::npos )
	    {
		token = line;
		line = "";
	    }
	    else
	    {
		token = line.substr( 0, idx );
		line.erase( 0, idx + 1 );
	    }
	    if ( token.size() )
	    {
		argv.push_back( token );
	    }
	}

	char **pt = new char *[argv.size() + 1];
	for ( vector<string>::size_type i = 0; i < argv.size(); i++ )
	{
	    pt[i] = new char[ argv[i].size()+1 ];
	    strcpy( pt[i], argv[i].c_str() );
	}
	pt[argv.size()] = NULL;
	execv( path.c_str(), pt );
	perror("execv :");
    }

    return 0;
}

int
Wizard::run_ts()
{
    std::cout << "Sorry, Starting TerraSync is not yet supported." << std::endl;
    return 1;
}

void
Wizard::stopProcess( long pid )
{
}

void
Wizard::stdout_cb( int fd, void* p )
{
    static_cast<Wizard*>(p)->stdout_cb( fd );
}

void
Wizard::stdout_cb( int fd )
{
    char buf[256];
    ssize_t n = read( fd, buf, sizeof( buf ) - 1 );
    if (n > 0)
    {
	buf[n] = 0;
 	logwin->append( buf );
    }
    else
    {
	Fl::remove_fd( fd );
	close( fd );

	int status;
	int pid = wait( &status );
    }
}

// void
// Wizard::show_log_window()
// {
//     win->show();
// }
