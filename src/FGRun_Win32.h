// FGRun_Win32.h -- Win32 implementation of fgrun user interface.
//
// Written by Bernie Bright, started Sep 2003.
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

#ifndef FGRUN_WIN32_H
#define FGRUN_WIN32_H

#include "UserInterface.h"

class FGRun_Win32 : public UserInterface
{
public:
    FGRun_Win32();
    ~FGRun_Win32();

private:
    void run_fgfs_impl();
};

#endif // FGRUN_WIN32_H
