#include <FL/Fl.h>
#include "FGRunUI.h"

int
main( int argc, char* argv[] )
{
    FGRunUI ui;

    ui.load_settings();
    ui.update_aircraft();
    ui.update_airports();

    ui.show();
    return Fl::run();
}
