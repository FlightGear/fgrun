EXTRA_DIST =
MOSTLYCLEANFILES =
CLEANFILES =

if WIN32
platform_srcs = os_win32.h run_win32.cxx
else
platform_srcs = os_posix.h run_posix.cxx fgrun_pty.cxx fgrun_pty.h
endif

BUILT_SOURCES = wizard.cxx wizard.h advanced.cxx advanced.h

bin_PROGRAMS = fgrun

fgrun_SOURCES = \
	wizard.cxx wizard.h wizard_funcs.cxx \
	advanced.cxx advanced.h advanced_funcs.cxx \
	AirportBrowser.cxx AirportBrowser.h apt_dat.h \
	AirportTable.cxx AirportTable.h \
	Fl_Table.cxx Fl_Table.H \
	Fl_Table_Row.cxx Fl_Table_Row.H \
	Fl_Plib.cxx Fl_Plib.h \
	Fl_Heading_Dial.cxx Fl_Heading_Dial.H \
	main.cxx \
	io.cxx \
	fgfsrc.cxx \
	logwin.cxx logwin.h \
	settings.cxx \
	util.cxx util.h \
	$(platform_srcs)

simgear_libs = -lsgmodel -lsgmath -lsgscreen -lsgprops -lsgxml -lsgmisc -lsgdebug -lsgstructure 
plib_libs = -lplibssg -lplibsg -lplibul

fgrun_LDADD = $(simgear_libs) $(plib_libs)

FLUID = @FLUID@

wizard.cxx wizard.h: wizard.fl
	$(FLUID) -c $(srcdir)/wizard.fl

advanced.cxx advanced.h: advanced.fl
	$(FLUID) -c $(srcdir)/advanced.fl

EXTRA_DIST += \
	wizard.fl \
	advanced.fl \
	folder_open.xpm
