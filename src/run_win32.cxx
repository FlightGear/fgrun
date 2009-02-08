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

static
int run_program( char *cmd, long &pid, char *env = 0 )
{
    //SECURITY_ATTRIBUTES procAttrs;
    //SECURITY_ATTRIBUTES threadAttrs;
    BOOL inheritHandles = TRUE;
    DWORD creationFlags = CREATE_NEW_PROCESS_GROUP;
#ifdef MINIDUMP
    creationFlags = creationFlags | DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS;
#endif
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
        env, // lpEnvironment
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
	pid = pi.dwProcessId;

#ifdef MINIDUMP
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
#else
        WaitForSingleObject( pi.hProcess, INFINITE );

        DWORD exitCode;
        GetExitCodeProcess( pi.hProcess, &exitCode );
#endif

        // Close process and thread handles. 
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
    }

    delete[] cmd;
    pid = 0;

    return exit_code;
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

    std::auto_ptr<char> childenv;
    int iVal;
    prefs.get( "env-count", iVal, 0 );
    if ( iVal > 0 )
    {
	LPTCH env = GetEnvironmentStrings();
	char *p = env;
	std::vector<std::string> strangevars;
	std::map<std::string,std::string> vars;
	while ( *p != '\0' )
	{
	    std::string var = p;
	    size_t pos = var.find( '=' );
	    if (pos == std::string::npos || pos == 0)
		strangevars.push_back( var );
	    else
		vars[ var.substr( 0, pos ) ] = var.substr( pos + 1 );
	    p += strlen( p ) + 1;
	}
	FreeEnvironmentStrings( env );

	const int buflen = FL_PATH_MAX;
	char buf[ buflen ];
	for (int i = 1; i <= iVal; ++i)
	{
	    buf[0] = 0;
	    prefs.get( Fl_Preferences::Name("env-var-%d", i),
			buf, "", buflen-1 );
	    std::string var = buf;
	    size_t pos = var.find( '=' );
	    if ( pos != std::string::npos && pos > 0 && pos < var.size() - 1 )
		vars[ var.substr( 0, pos ) ] = var.substr( pos + 1 );
	}
	size_t len = 1;
	for ( std::vector<std::string>::iterator ii = strangevars.begin(); ii != strangevars.end(); ++ii )
	{
	    len += ii->size();
	    len += 1;
	}
	for ( std::map<std::string,std::string>::iterator ii = vars.begin(); ii != vars.end(); ++ii )
	{
	    len += ii->first.size();
	    len += ii->second.size();
	    len += 2;
	}

	childenv.reset( new char[ len ] );
	p = childenv.get();
	for ( std::vector<std::string>::iterator ii = strangevars.begin(); ii != strangevars.end(); ++ii )
	{
	    strcpy( p, ii->c_str() );
	    p += strlen( p ) + 1;
	}
	for ( std::map<std::string,std::string>::iterator ii = vars.begin(); ii != vars.end(); ++ii )
	{
	    strcpy( p, ii->first.c_str() );
	    strcat( p, "=" );
	    strcat( p, ii->second.c_str() );
	    p += strlen( p ) + 1;
	}
	*p = '\0';
    }

    return run_program( cmd, fgPid, childenv.get() );
}

int
Wizard::run_ts()
{
    if ( tsPid != 0 )
	return -1;

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
    char *p = strrchr( exe, '/' );
    if ( p == 0 )
	p = strrchr( exe, '\\' );
    if ( p == 0 )
	return 1;

    strcpy( p + 1, "terrasync.exe" );

    int iVal;
    prefs.get( "ts_dir", iVal, 0 );
    if ( iVal == 0 )
	return 1;

    prefs.get( "fg_scenery", buf, "", buflen-1 );
    string_list dirlist = sgPathSplit( buf );
    if ( dirlist.empty() )
        return 1;

    std::cout << iVal << " - '" << dirlist[iVal-1] << "'" << std::endl;

    int port;
    prefs.get( "terrasync_port", port, 5505 );
    std::ostringstream oss;
    oss << exe << " -S -d \"" << dirlist[iVal-1] << "\" -p " << port;

    char* cmd = new char[ oss.str().length() + 1 ];
    strcpy( cmd, oss.str().c_str() );
    std::cout << cmd << std::endl;

    return run_program( cmd, tsPid );
}

void
Wizard::stopProcess( long pid )
{
    if (pid != 0)
	GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, pid );
}
