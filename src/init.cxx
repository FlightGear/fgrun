#include <sstream>
#include <FL/filename.h>
#include <FL/Fl_File_Chooser.h>

#include "UserInterface.h"
#include "airportdb.h"

void load_airportdb_cb( void* );

void
UserInterface::init()
{
    airportdb_ = new AirportDB;

    page_list->add("General");
    page_list->add("Features");
    page_list->add("Flight Model");
    page_list->add("Freeze");
    page_list->add("Initial Position");
    page_list->add("Rendering");
    page_list->add("Time");
    page_list->add("Network");
    page_list->add("Input/Output");
    page_list->add("Avioncs");
    page_list->add("Properties");
    page_list->add("Debugging");
    page_list->add("Environment");
    page_list->add("Airport Browser");
    page_list->select(1);
    show_page(0);

    load_settings_cb();

    if (fg_root->size() > 0
	&& fl_filename_isdir(fg_root->value()))
    {
	Fl::add_idle(update_aircraft_cb, this);
    }

    update_airports_cb();
}

void
UserInterface::fg_exe_cb()
{
    char* p = fl_file_chooser("Select executable", 0, fg_exe->value(), 0);
    if (p != 0)
    {
	fg_exe->value( p );
	run->activate();
    }
}

void
UserInterface::fg_root_cb()
{
    char* p = fl_dir_chooser( "Select FG_ROOT directory",
			      fg_root->value(), 0);
    if (p != 0)
    {
	fg_root->value( p );
	std::ostringstream oss;
	oss << p << "/Scenery";

	if (fl_filename_isdir( oss.str().c_str() ))
	{
	    fg_scenery->value( oss.str().c_str() );
	    airport_update->activate();
	    aircraft_update->activate();
	}
    }
}

void
UserInterface::fg_scenery_cb()
{
    char* p = fl_dir_chooser("Select FG_SCENERY directory",
			     fg_scenery->value(), 0);
    if (p != 0)
    {
	fg_scenery->value( p );
	airport_update->activate();
	aircraft_update->activate();
    }
}

void
UserInterface::fg_browser_cb()
{
    char* p = fl_file_chooser("Select browser", 0, browser->value(), 0);
    if (p != 0)
	browser->value( p );
}
