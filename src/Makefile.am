bin_PROGRAMS = fgrun

fgrun_SOURCES = \
	main.cxx \
	FGRunUI.cxx FGRunUI.h \
	ui.cxx ui.h

fgrun_LDADD = @X_LIBS@

ui.cxx ui.h: ui.fl
	@FLUID_COMPILE@ ui.fl

##MAINTAINERCLEANFILES = ui.cxx ui.h
EXTRA_DIST = ui.fl
