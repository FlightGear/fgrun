#ifndef FGRUNUI_H_INCLUDED
#define FGRUNUI_H_INCLUDED

#include <string>
#include <vector>
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

    void output_save();
    void output_save_as();

protected:
    int set_choice( Fl_Choice* w, const char* s );
    void write_dot_fgfsrc();
    
protected:
    std::string default_aircraft;
    std::string default_airport;
    std::vector< std::string > airports;
    //std::vector< std::string > dirs;

    bool modflag;
    int io_options_list_value;
    std::string output_fname;

private:
    void scan_for_airports( const char* dir );
    //friend void idle_cb( void* );
};

#endif // FGRUNUI_H_INCLUDED
