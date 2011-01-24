
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

if (MSVC)
	set( simgear_libs ${SIMGEAR_LIBRARIES} )
else (MSVC)
	set( simgear_libs
		${SIMGEAR_MODEL_LIBRARY}
		${SIMGEAR_SCREEN_LIBRARY}
		${SIMGEAR_PROPS_LIBRARY}
		${SIMGEAR_XML_LIBRARY}
		${SIMGEAR_DEBUG_LIBRARY}
		${SIMGEAR_BVH_LIBRARY}
		${SIMGEAR_MATERIAL_LIBRARY}
		${SIMGEAR_MODEL_LIBRARY}
		${SIMGEAR_UTIL_LIBRARY}
		${SIMGEAR_STRUCTURE_LIBRARY}
		${SIMGEAR_PROPS_LIBRARY}
		${SIMGEAR_TGDB_LIBRARY}
		${SIMGEAR_MATH_LIBRARY}
		${SIMGEAR_MISC_LIBRARY}
		${SIMGEAR_BVH_LIBRARY}
		${SIMGEAR_IO_LIBRARY}
		${SIMGEAR_BUCKET_LIBRARY}
		${SIMGEAR_MODEL_LIBRARY}
		${SIMGEAR_UTIL_LIBRARY}
		)
endif (MSVC)

add_executable(fgrun ${SOURCES})

target_link_libraries(fgrun 
    ${SOCKETS_LIBRARY}
    ${simgear_libs}
    ${OPENSCENEGRAPH_LIBRARIES} 
    ${FLTK_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${INTL_LIBRARY}
    ${X11_Xinerama_LIB}
    ${X11_Xft_LIB}
    )

install(TARGETS fgrun RUNTIME DESTINATION bin)
