if WIN32
platform_srcs = FGRun_Win32.cxx FGRun_Win32.h
else
platform_srcs = FGRun_Posix.cxx FGRun_Posix.h
endif

bin_PROGRAMS = fgrun

fgrun_SOURCES = \
	main.cxx \
	settings.cxx \
	init.cxx \
	fgfsrc.cxx \
	$(platform_srcs) \
	UserInterface.cxx UserInterface.h

if WIN32
win32_libs = -luuid -lwsock32 -lole32 -lwinmm -lgdi32
else
win32_libs =
endif

fgrun_LDADD = -lfltk $(win32_libs)


FLUID = @FLUID@

UserInterface.cxx UserInterface.h: UserInterface.fl
	$(FLUID) -c UserInterface.fl

##MAINTAINERCLEANFILES = UserInterface.cxx UserInterface.h
EXTRA_DIST = UserInterface.fl
