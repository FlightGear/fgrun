#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#include <string>

#include "FGRun_Posix.h"

using std::string;

FGRun_Posix::FGRun_Posix()
{
}

FGRun_Posix::~FGRun_Posix()
{
}

void
FGRun_Posix::run_fgfs_impl()
{
    pid_t pid = fork();
    if (pid < 0)
    {
	perror( "fork error" );
	return;
    }

    if (pid > 0)
    {
	// parent
	return;
    }
    else
    {
	// child
	string path = fg_exe->value();
	string arg0;
	string::size_type idx = path.find_last_of( "/\\" );
	if (idx != string::npos)
	{
	    arg0 = path.substr( idx+1 );
	}
	else
	{
	    arg0 = path;
	}

	execl( path.c_str(), arg0.c_str(), NULL );
    }
}

