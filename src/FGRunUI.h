#ifndef FGRUNUI_H_INCLUDED
#define FGRUNUI_H_INCLUDED

#include <string>
#include "ui.h"

using std::string;

class FGRunUI : public UserInterface
{
public:
    FGRunUI();
    ~FGRunUI();

    void load_settings();
    void save_settings();
    void update_aircraft();
    void update_airports();
    void select_fg_exe();
    void select_fg_root();
    void select_fg_scenery();
    void run_fgfs();
    void select_browser();
    void add_io_item();
    void edit_io_item();
    void delete_io_item();

    void OptionDlg_ok_cb();
    void io_medium_cb( Fl_Choice*, void* );
    void reset();

private:
    void stdout_cb_i( int fd );
    static void stdout_cb( int fd, void* );
    void stderr_cb_i( int fd );
    static void stderr_cb( int fd, void* );
    
private:
    string default_aircraft;
    string default_airport;
    bool modflag;
    int io_options_list_value;
};

#endif // FGRUNUI_H_INCLUDED
