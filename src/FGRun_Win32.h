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
