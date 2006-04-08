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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "wizard.h"
#include <windows.h>
#include <FL/filename.h>

using std::string;

static
int encase_arg( string & line, string arg ) {
  int iret = 0;
  string ar = "--"; // start option argument
  string are = " --"; // to next, if any
  ar += arg; // add the current argument/option
  ar += "="; // add EQUALS
  size_t pos1 = line.find(ar); // find, like '--fg-root='
  if( pos1 != string::npos ) { // if FOUND
     size_t sz = pos1 + ar.size(); // get the arg size
     size_t pos2 = line.find( are, sz ); // find next arg beginning
     if( pos2 == string::npos ) { // if NOT FOUND
        pos2 = line.size();
     }
     line.insert( pos2, "\"" ); // pop in the quotes, at the end first
     line.insert( sz, "\"" ); // then at the front of the 'path'
     iret = 1; // advise done
  }
  return iret;
}

void
Wizard::run_fgfs(const string &args)
{
    const int buflen = FL_PATH_MAX;
    char exe[ buflen ];
    char buf[ buflen ];

    prefs.get( "fg_exe", buf, "", buflen-1 );
    fl_filename_absolute( exe, buf );
    // Convert cygwin path ( if any ) into real windows path
    if ( strncmp( exe, "/cygdrive", 9 ) == 0 ) {
        memmove( exe, exe+9, strlen(exe)-8 );
        exe[0] = exe[1];
        exe[1] = ':';
    }

    string line = args;
    encase_arg( line, "fg-root" );
    encase_arg( line, "fg-scenery" );

    char* cmd = new char[ strlen(exe) +
                          line.size() + 2 ];
    strcpy( cmd, exe );
    strcat( cmd, " " );
    strcat( cmd, line.c_str() );

    //SECURITY_ATTRIBUTES procAttrs;
    //SECURITY_ATTRIBUTES threadAttrs;
    BOOL inheritHandles = TRUE;
    DWORD creationFlags = 0;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset( &si, 0, sizeof(si) );
    si.cb = sizeof(si);

    BOOL bStarted = ::CreateProcess( NULL, // lpApplicationName
        cmd, // lpCommandLine
        NULL, // lpProcessAttributes
        NULL, // lpThreadAttributes
        inheritHandles,
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
    else
    {
        WaitForSingleObject( pi.hProcess, INFINITE );

        DWORD exitCode;
        GetExitCodeProcess( pi.hProcess, &exitCode );

        // Close process and thread handles. 
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
    }

    delete[] cmd;
}

