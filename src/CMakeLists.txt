
if (WIN32)
    set( platform_srcs run_win32.cxx )
else (WIN32)
    set( platform_srcs run_posix.cxx fgrun_pty.cxx )
endif (WIN32)

set(SOURCES
	wizard.cxx wizard_funcs.cxx
	advanced.cxx advanced_funcs.cxx
	AirportBrowser.cxx
	AirportTable.cxx
	Fl_Table.cxx
	Fl_Table_Row.cxx
	Fl_OSG.cxx
	Fl_Heading_Dial.cxx
	main.cxx
	io.cxx
	fgfsrc.cxx
	logwin.cxx
	parkingloader.cxx
	settings.cxx
	util.cxx
	${platform_srcs}
	)

add_executable(fgrun ${SOURCES})

target_link_libraries(fgrun 
    ${SOCKETS_LIBRARY}
	${SIMGEAR_LIBRARIES}
	${OPENSCENEGRAPH_LIBRARIES} 
	${FLTK_LIBRARIES}
	${ZLIB_LIBRARIES}
    ${INTL_LIBRARY}
    )

install(TARGETS fgrun RUNTIME DESTINATION bin)
