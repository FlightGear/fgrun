bin_PROGRAMS = fgrun

fgrun_SOURCES = \
	main.cxx \
	FGRunUI.cxx FGRunUI.h \
	ui.cxx ui.h

##fgrun_LDADD = 

FLUID = @FLUID@

ui.cxx ui.h: ui.fl
	@FLUID@ -c ui.fl

##MAINTAINERCLEANFILES = ui.cxx ui.h
EXTRA_DIST = ui.fl
