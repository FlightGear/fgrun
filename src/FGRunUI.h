#ifndef FGRUNUI_H_INCLUDED
#define FGRUNUI_H_INCLUDED

#include <string>
#include "ui.h"

/**
 * 
 */
class FGRunUI : public UserInterface
{
public:
    FGRunUI();
    virtual ~FGRunUI();

    virtual void run_fgfs() = 0;

    void load_settings();
    void save_settings();
    void update_aircraft();
    void update_airports();
    void select_fg_exe();
    void select_fg_root();
    void select_fg_scenery();
    void select_browser();
    void add_io_item();
    void edit_io_item();
    void delete_io_item();

    void OptionDlg_ok_cb();
    void io_medium_cb( Fl_Choice*, void* );
    void reset();

protected:
    int set_choice( Fl_Choice* w, const char* s );
    void write_dot_fgfsrc();
    
protected:
    std::string default_aircraft;
    std::string default_airport;
    bool modflag;
    int io_options_list_value;
};

#endif // FGRUNUI_H_INCLUDED
