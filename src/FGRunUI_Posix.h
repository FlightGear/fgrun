#ifndef FGRUNUI_POSIX_H
#define FGRUNUI_POSIX_H

#include "FGRunUI.h"

/**
 * 
 */
class FGRunUI_Posix : public FGRunUI
{
public:
    void run_fgfs();

private:
    void stdout_cb_i( int fd );
    static void stdout_cb( int fd, void* );
    void stderr_cb_i( int fd );
    static void stderr_cb( int fd, void* );
};

#endif // FGRUNUI_POSIX_H
