EXTRA_DIST =
MOSTLYCLEANFILES =
CLEANFILES =

if WIN32
platform_srcs = FGRun_Win32.cxx FGRun_Win32.h
else
platform_srcs = FGRun_Posix.cxx FGRun_Posix.h fgrun_pty.cxx fgrun_pty.h
endif

BUILT_SOURCES = UserInterface.cxx UserInterface.h

bin_PROGRAMS = fgrun

fgrun_SOURCES = \
	AirportBrowser.cxx AirportBrowser.h \
	Fl_Table.cxx Fl_Table.H \
	Fl_Table_Row.cxx Fl_Table_Row.H \
	airportdb.cxx airportdb.h \
	main.cxx \
	settings.cxx \
	init.cxx \
	fgfsrc.cxx \
	airport.cxx \
	aircraft.cxx \
	io.cxx \
	util.cxx \
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
	$(FLUID) -c $(srcdir)/UserInterface.fl

##MAINTAINERCLEANFILES = UserInterface.cxx UserInterface.h
EXTRA_DIST += \
	UserInterface.fl

