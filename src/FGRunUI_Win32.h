#ifndef FGRUNUI_WIN32_H
#define FGRUNUI_WIN32_H

#include "FGRunUI.h"
// #include "Redirect.h"

// class CRedir : public CRedirect
// {
// protected:
//     void OnChildWrite( UINT OutputID, LPCSTR lpszOutput );
// protected:
//     virtual void OnChildStarted( LPCSTR lpszCmdLine );
//     virtual void OnChildStdOutWrite( LPCSTR lpszBuffer );
//     virtual void OnChildStdErrWrite( LPCSTR lpszBuffer );
//     virtual void OnChildTerminate();
// };

/**
 * 
 */
class FGRunUI_Win32 : public FGRunUI
{
public:
    void run_fgfs();
private:
//     CRedir m_Redirect;
};

#endif // FGRUNUI_WIN32_H
