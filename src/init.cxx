#include <FL/filename.h>
#include "UserInterface.h"

void load_airportdb_cb( void* );

void
UserInterface::init()
{
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
    page_list->add("Airport Finder");
    page_list->select(1);
    show_page(0);

    load_settings_cb();

    if (fg_root->size() > 0
	&& fl_filename_isdir(fg_root->value()))
    {
	Fl::add_idle(update_aircraft_cb, this);
	Fl::add_idle(load_airportdb_cb, this);
    }

    update_airports_cb();
}
