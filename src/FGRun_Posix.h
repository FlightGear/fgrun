#ifndef FGRUN_POSIX_H
#define FGRUN_POSIX_H

#include "UserInterface.h"

class FGRun_Posix : public UserInterface
{
public:
    FGRun_Posix();
    ~FGRun_Posix();

private:
    void run_fgfs_impl();
};

#endif // FGRUN_POSIX_H
