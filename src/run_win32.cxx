// FGRun_Win32.cxx -- 
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

#include "wizard.h"
#include <windows.h>
#include <FL/filename.h>

void
Wizard::run_fgfs()
{
    const int buflen = FL_PATH_MAX;
    char exe[ buflen ];
    char root[ buflen ];
    char buf[ buflen ];
    char aircraft[128];

    prefs.get( "fg_exe", buf, "", buflen-1 );
    fl_filename_absolute( exe, buf );

    prefs.get( "fg_root", buf, "", buflen-1 );
    fl_filename_absolute( root, buf );

    prefs.get( "aircraft", aircraft, "", sizeof(aircraft)-1);

    char* cmd = new char[ strlen(exe) +
                          strlen(root) +
                          strlen(aircraft) + 26 ];
    strcpy( cmd, exe );
    strcat( cmd, " --fg-root=\"" );
    strcat( cmd, root );
    strcat( cmd, "\" --aircraft=" );
    strcat( cmd, aircraft );

    //SECURITY_ATTRIBUTES procAttrs;
    //SECURITY_ATTRIBUTES threadAttrs;
    BOOL inheritHandles = FALSE;
    DWORD creationFlags = 0;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset( &si, 0, sizeof(si) );
    si.cb = sizeof(si);

    BOOL bStarted = ::CreateProcess( NULL, // lpApplicationName
        cmd, // lpCommandLine
        NULL, // lpProcessAttributes
        NULL, // lpThreadAttributes
        FALSE,
        creationFlags,
        NULL, // lpEnvironment
        NULL, // lpCurrentDirectory
        &si,
        &pi
        );
    if (!bStarted)
    {
        LPVOID lpMsgBuf;
        FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL 
        );
        MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
        // Free the buffer.
        LocalFree( lpMsgBuf );
    }

    delete[] cmd;
}


