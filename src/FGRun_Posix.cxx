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

#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#include <string>

#include "FGRun_Posix.h"

using std::string;

FGRun_Posix::FGRun_Posix()
{
}

FGRun_Posix::~FGRun_Posix()
{
}

void
FGRun_Posix::run_fgfs_impl()
{
    pid_t pid = fork();
    if (pid < 0)
    {
	perror( "fork error" );
	return;
    }

    if (pid > 0)
    {
	// parent
	return;
    }
    else
    {
	// child
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

