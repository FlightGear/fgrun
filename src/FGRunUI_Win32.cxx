#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/////////////////////////////////////////////////////////////////////////////
// API Function
BOOL WINAPI IsWinNT()
{
    // get windows version
    DWORD WindowsVersion = GetVersion();
    DWORD WindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(WindowsVersion)));
    DWORD WindowsMinorVersion = (DWORD)(HIBYTE(LOWORD(WindowsVersion)));

    // Running on WIN9x ?
    if (WindowsVersion >= 0x80000000) return FALSE;
  
    // Running on NT
    return TRUE;
}

void
FGRunUI_Win32::run_fgfs()
{
    this->write_dot_fgfsrc();

    HANDLE hProcess = ::GetCurrentProcess();

    PROCESS_INFORMATION pi;

    // Set up the start up info struct.
    STARTUPINFO si;
    ::ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    //si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.dwFlags = STARTF_USESHOWWINDOW;
    //si.hStdOutput = hStdOut;
    //si.hStdInput  = hStdIn;
    //si.hStdError  = hStdErr;
	
    // Use this if you want to show the child.
    //si.wShowWindow = bShowChildWindow ? SW_SHOW: SW_HIDE;
    si.wShowWindow = SW_HIDE;
    // Note that dwFlags must include STARTF_USESHOWWINDOW if you want to
    // use the wShowWindow flags.

    // Create the NULL security token for the process
    LPVOID lpSD = NULL;
    LPSECURITY_ATTRIBUTES lpSA = NULL;

    // On NT/2000 the handle must have PROCESS_QUERY_INFORMATION access.
    // This is made using an empty security descriptor. It is not the same
    // as using a NULL pointer for the security attribute!

    if (IsWinNT())
    {
	lpSD = ::GlobalAlloc(GPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	::InitializeSecurityDescriptor((PSECURITY_DESCRIPTOR)lpSD, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl((PSECURITY_DESCRIPTOR)lpSD, -1, 0, 0);

	lpSA = (LPSECURITY_ATTRIBUTES)::GlobalAlloc(GPTR, sizeof(SECURITY_ATTRIBUTES));
	lpSA->nLength = sizeof(SECURITY_ATTRIBUTES);
	lpSA->lpSecurityDescriptor = lpSD;
	lpSA->bInheritHandle = TRUE;
    }
    LPCSTR lpszCmdLine = fg_exe->value();
    // Try to spawn the process.
    BOOL bResult = ::CreateProcess(NULL, (char*)lpszCmdLine, lpSA, NULL, TRUE,
				   CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

    // Cleanup memory allocation
    if (lpSA != NULL)
	::GlobalFree(lpSA);
    if (lpSD != NULL)
	::GlobalFree(lpSD);

    // Return if an error occurs.
//     if (!bResult) return FALSE;

    // Close any unnecessary handles.
    ::CloseHandle(pi.hThread);

    // Save global child process handle to cause threads to exit.
//     return pi.hProcess;
}
