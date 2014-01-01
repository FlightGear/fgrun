
if (WIN32)
    set( platform_srcs run_win32.cxx )
	set( platform_incs os_win32.h )
else (WIN32)
    set( platform_srcs run_posix.cxx fgrun_pty.cxx )
	set( platform_incs os_posix.h )
endif (WIN32)

if (MSVC)
    set( RESOURCE_FILE fgrun.rc )
endif (MSVC)

FLTK_WRAP_UI(Wizard wizard.fl)
add_library(Wizard ${Wizard_FLTK_UI_SRCS})

FLTK_WRAP_UI(Advanced advanced.fl)
add_library(Advanced ${Advanced_FLTK_UI_SRCS})

set( ui_libs_1_3 Wizard Advanced )

set(SOURCES
    wizard_funcs.cxx
    advanced_funcs.cxx
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
    ${RESOURCE_FILE}
    )
	
set(HEADERS
    AirportBrowser.h
    AirportTable.h
    Fl_Table.H
    Fl_Table_Row.H
    Fl_OSG.h
    Fl_Heading_Dial.H
    logwin.h
    parkingloader.h
    util.h
    ${platform_incs}
	)

set( simgear_libs
     ${SIMGEAR_LIBRARIES}
     ${SIMGEAR_CORE_LIBRARY_DEPENDENCIES}
     ${SIMGEAR_SCENE_LIBRARY_DEPENDENCIES})

add_executable(fgrun ${SOURCES} ${HEADERS})

add_dependencies(fgrun Wizard)
add_dependencies(fgrun Advanced)

target_link_libraries(fgrun
    ${ui_libs_1_3}
    ${WINSOCK_LIBRARY}
    ${simgear_libs}
    ${OPENSCENEGRAPH_LIBRARIES} 
    ${FLTK_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${INTL_LIBRARY}
    ${OPENPTY_LIBRARY}
    ${PLATFORM_LIBS}
    )

install(TARGETS fgrun RUNTIME DESTINATION bin)
