// fgrun_pty.cxx -- Portable pseudo-tty functions.
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#if defined(HAVE_PTY_H)
#  include <pty.h>
#endif

#if defined(HAVE_UTMP_H)
#  include <utmp.h>
#endif

#if defined(HAVE_UNISTD_H)
#  include <unistd.h>
#endif

/**
 * 
 */
int
pty_open( int* amaster,
	  int* aslave,
	  char* name,
	  struct termios* termp,
	  struct winsize* winp )
{
#if defined(HAVE_OPENPTY) || defined(BSD4_4)

    return openpty( amaster, aslave, name, termp, winp );

#elif defined(HAVE__GETPTY)

    // Open the master pty device.
    int master;
    char* pts_name = _getpty( &master, O_RDWR, 0622, 0 );
    if (pts_name == 0)
    {
	return -1;
    }

    // Open the slave pty device.
    int pts_fd = open( pts_name, O_RDWR );
    if (pts_fd < 0)
    {
	close( master );
	return -1;
    }
    *amaster = master;
    *aslave = pts_fd;
    strcpy( name, pts_name );
    return 0;

#elif defined(HAVE_DEV_PTMX)

    int ptm = open( "/dev/ptmx", O_RDWR );
    if (ptm < 0)
    {
	return -1;
    }
    if (grantpt( ptm ) < 0)
    {
	close( ptm );
	return -1;
    }
    if (unlockpt( ptm ) < 0)
    {
	close( ptm );
	return -1;
    }

    char* pts_name = ptsname( ptm );
    if (pts_name == 0)
    {
	close( ptm );
	return -1;
    }

    int pts = open( pts_name, O_RDWR );
    if (pts < 0)
    {
	close( ptm );
	return -1;
    }

//     ioctl( pts, I_PUSH, "ptem" );
//     ioctl( pts, I_PUSH, "ldterm" );
//     ioctl( pts, I_PUSH, "ttcompat" );

    *amaster = ptm;
    *aslave = pts;
    strcpy( name, pts_name );
    return 0;

#elif defined(HAVE_DEV_PTS)
    return -1; // TODO
#else
    // BSD-style pty code
    return -1; // TODO
#endif
}

/**
 * 
 */
int
tty_login( int fd )
{
#if defined(HAVE_LOGIN_TTY)

    return login_tty( fd );

#else

#endif
}

/**
 * 
 */
pid_t
pty_fork( int* amaster,
	  char* name,
	  struct termios* termp,
	  struct winsize* winp )
{
    int master;
    int slave;

    if (pty_open( &master, &slave, name, termp, winp ) < 0)
	return -1;

    pid_t pid = fork();
    if (pid < 0)
    {
	return -1;
    }
    else if (pid == 0)
    {
	// Child.
	close( master );
	if (tty_login( slave ) < 0)
	    _exit(1);
	return 0;
    }
    else
    {
	// Parent.
	*amaster = master;
	close( slave );
	return pid;
    }
}
