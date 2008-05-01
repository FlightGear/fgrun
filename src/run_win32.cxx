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
#include <dbghelp.h>
#include <FL/filename.h>
#include <sstream>
#include <time.h>

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

int
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
    encase_arg( line, "config" );
    encase_arg( line, "browser-app" );

    char* cmd = new char[ strlen(exe) +
                          line.size() + 2 ];
    strcpy( cmd, exe );
    strcat( cmd, " " );
    strcat( cmd, line.c_str() );

    //SECURITY_ATTRIBUTES procAttrs;
    //SECURITY_ATTRIBUTES threadAttrs;
    BOOL inheritHandles = TRUE;
    DWORD creationFlags = DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    int exit_code = 0;

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
        DWORD dwContinueStatus = DBG_CONTINUE; // exception continuation 
        bool end = false, dump = false;

        while ( !end )
        {
            DEBUG_EVENT DebugEv;

            // Wait for a debugging event to occur. The second parameter indicates
            // that the function does not return until a debugging event occurs. 
            WaitForDebugEvent( &DebugEv, INFINITE ); 

            switch ( DebugEv.dwDebugEventCode ) 
            {
            case EXCEPTION_DEBUG_EVENT:
                switch( DebugEv.u.Exception.ExceptionRecord.ExceptionCode )
                {
                case EXCEPTION_BREAKPOINT:
                    break;
                case EXCEPTION_ACCESS_VIOLATION:
                case EXCEPTION_DATATYPE_MISALIGNMENT:
                case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                case EXCEPTION_FLT_DENORMAL_OPERAND:
                case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                case EXCEPTION_FLT_INEXACT_RESULT:
                case EXCEPTION_FLT_INVALID_OPERATION:
                case EXCEPTION_FLT_OVERFLOW:
                case EXCEPTION_FLT_STACK_CHECK:
                case EXCEPTION_FLT_UNDERFLOW:
                case EXCEPTION_INT_DIVIDE_BY_ZERO:
                case EXCEPTION_INT_OVERFLOW:
                case EXCEPTION_PRIV_INSTRUCTION:
                case EXCEPTION_IN_PAGE_ERROR:
                case EXCEPTION_ILLEGAL_INSTRUCTION:
                case EXCEPTION_NONCONTINUABLE_EXCEPTION:
                case EXCEPTION_STACK_OVERFLOW:
                case EXCEPTION_INVALID_DISPOSITION:
                case EXCEPTION_GUARD_PAGE:
                case EXCEPTION_INVALID_HANDLE:
                    std::cout << "Exception detected (0x" << std::hex << DebugEv.u.Exception.ExceptionRecord.ExceptionCode << std::dec << ") : " << DebugEv.dwThreadId << std::endl;

                    if ( !dump )
                    {
                        std::cout << "Creating dump file" << std::endl;
                        time_t now = time(0);
                        ::tm *stm = gmtime( &now );

                        const int buflen = FL_PATH_MAX;
                        char buf[ buflen ];
                        prefs.getUserdataPath( buf, sizeof(buf) );

                        std::ostringstream os;
                        os << buf << "fgfs-dump-" << DebugEv.dwProcessId
                            << "-" << ( 1900 + stm->tm_year )
                            << "-" << ( 1 + stm->tm_mon )
                            << "-" << stm->tm_mday
                            << "-" << stm->tm_hour
                            << "-" << stm->tm_min
                            << "-" << stm->tm_sec
                            << ".dmp";
                        dump_file_name = os.str();

                        HANDLE hFile = CreateFile( dump_file_name.c_str(), GENERIC_READ | GENERIC_WRITE,
                                                    0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
                        if ( hFile != INVALID_HANDLE_VALUE )
                        {
                            MiniDumpWriteDump( pi.hProcess, DebugEv.dwProcessId, hFile,
                                                MiniDumpNormal, 0, 0, 0 ); 
                            CloseHandle( hFile );
                            char *pbuf = new char[ dump_file_name.size() * 2 + 1 ];
                            CharToOem( dump_file_name.c_str(), pbuf );
                            std::cout << "Dump file created (" << pbuf << ")" << std::endl;
                            delete pbuf;
                            dump = true;
                        }
                    }

                    dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
                    exit_code = 1;
                    break;
                }
                break;
            case EXIT_PROCESS_DEBUG_EVENT:
                std::cout << "Exit process detected" << std::endl;
                end = true;
                break;
            default:
                break;
            }

            // Resume executing the thread that reported the debugging event. 
            ContinueDebugEvent( DebugEv.dwProcessId, DebugEv.dwThreadId, dwContinueStatus );
        }

        // Close process and thread handles. 
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
    }

    delete[] cmd;

    return exit_code;
}

