// fgrun_posix.h -- POSIX implementation of fgrun user interface.
//
// Written by Bernie Bright, started Sep 2003.
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

#ifndef FGRUN_POSIX_H
#define FGRUN_POSIX_H

#include "UserInterface.h"

class FGOutputWindow;

class FGRun_Posix : public UserInterface
{
public:
    FGRun_Posix();
    ~FGRun_Posix();

private:
    void run_fgfs_impl();

    static void stdout_cb( int fd, void* arg );
    void stdout_cb( int fd );

    static void stderr_cb( int fd, void* arg );
    void stderr_cb( int fd );

private:
    FGOutputWindow* win;
};

#endif // FGRUN_POSIX_H
