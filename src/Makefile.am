if WIN32
platform_srcs = FGRunUI_Win32.cxx FGRunUI_Win32.h
else
platform_srcs = FGRunUI_Posix.cxx FGRunUI_Posix.h
endif

bin_PROGRAMS = fgrun

fgrun_SOURCES = \
	main.cxx \
	FGRunUI.cxx FGRunUI.h \
	$(platform_srcs) \
	settings.cxx \
	ui.cxx ui.h

if WIN32
win32_libs = -luuid -lwsock32 -lole32 -lwinmm -lgdi32
else
win32_libs =
endif

fgrun_LDADD = -lfltk $(win32_libs)


FLUID = @FLUID@

ui.cxx ui.h: ui.fl
	$(FLUID) -c ui.fl

##MAINTAINERCLEANFILES = ui.cxx ui.h
EXTRA_DIST = ui.fl
