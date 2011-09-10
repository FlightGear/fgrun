
if (WIN32)
    set( platform_srcs run_win32.cxx )
else (WIN32)
    set( platform_srcs run_posix.cxx fgrun_pty.cxx )
endif (WIN32)

if (MSVC)
    if (CMAKE_CL_64)
        set( RESOURCE_FILE fgrun64.rc )
    else (CMAKE_CL_64)
        set( RESOURCE_FILE fgrun.rc )
    endif (CMAKE_CL_64)
endif (MSVC)

if (HAVE_FLTK_1_3)

    FLTK_WRAP_UI(Wizard wizard.fl)
    add_library(Wizard ${Wizard_FLTK_UI_SRCS})

    FLTK_WRAP_UI(Advanced advanced.fl)
    add_library(Advanced ${Advanced_FLTK_UI_SRCS})

    set( ui_libs_1_3 Wizard Advanced )

else (HAVE_FLTK_1_3)

    set( ui_src_1_1 wizard.cxx advanced.cxx )

endif (HAVE_FLTK_1_3)

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
    ${ui_src_1_1}
    ${RESOURCE_FILE}
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

if (HAVE_FLTK_1_3)
    add_dependencies(fgrun Wizard)
    add_dependencies(fgrun Advanced)
endif (HAVE_FLTK_1_3)

target_link_libraries(fgrun
    ${ui_libs_1_3}
    ${WINSOCK_LIBRARY}
    ${simgear_libs}
    ${OPENSCENEGRAPH_LIBRARIES} 
    ${FLTK_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${INTL_LIBRARY}
    ${X11_Xinerama_LIB}
    ${X11_Xft_LIB}
    ${OPENPTY_LIBRARY}
    )

install(TARGETS fgrun RUNTIME DESTINATION bin)
