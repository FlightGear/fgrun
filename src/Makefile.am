EXTRA_DIST =
MOSTLYCLEANFILES =
CLEANFILES =

if WIN32
platform_srcs = run_win32.cxx
else
platform_srcs = run_posix.cxx fgrun_pty.cxx fgrun_pty.h
endif

BUILT_SOURCES = wizard.cxx wizard.h advanced.cxx advanced.h

bin_PROGRAMS = fgrun

fgrun_SOURCES = \
	wizard.cxx wizard.h wizard_funcs.cxx \
	advanced.cxx advanced.h advanced_funcs.cxx \
	AirportBrowser.cxx AirportBrowser.h \
	AirportTable.cxx AirportTable.h \
	Fl_Table.cxx Fl_Table.H \
	Fl_Table_Row.cxx Fl_Table_Row.H \
	Fl_Plib.cxx Fl_Plib.h \
	main.cxx \
	io.cxx \
	fgfsrc.cxx \
	logwin.cxx logwin.h \
	settings.cxx \
	util.cxx util.h \
	$(platform_srcs)

if WIN32
win32_libs = -luuid -lwsock32 -lole32 -lwinmm -lgdi32
else
win32_libs =
endif

fgrun_LDADD = -lsgprops -lsgmisc -lsgxml -lsgstructure -lsgdebug -lfltk_gl -lfltk -lplibssg -lplibsg -lplibul -lGL $(win32_libs)

FLUID = @FLUID@

wizard.cxx wizard.h: wizard.fl
	$(FLUID) -c $(srcdir)/wizard.fl

advanced.cxx advanced.h: advanced.fl
	$(FLUID) -c $(srcdir)/advanced.fl

EXTRA_DIST += \
	wizard.fl \
	advanced.fl
