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
