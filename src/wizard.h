// generated by Fast Light User Interface Designer (fluid) version 1.0104

#ifndef wizard_h
#define wizard_h
#include <FL/Fl.H>
#include <string>
#include <iostream>
#include <FL/Fl_Preferences.H>
class LogWindow;
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Wizard.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Browser.H>
#include "Fl_Plib.h"
#include "AirportBrowser.h"

class Wizard {
public:
  Wizard() ;
private:
  Fl_Double_Window *win;
  Fl_Button *next;
  inline void cb_next_i(Fl_Button*, void*);
  static void cb_next(Fl_Button*, void*);
  Fl_Button *prev;
  inline void cb_prev_i(Fl_Button*, void*);
  static void cb_prev(Fl_Button*, void*);
  Fl_Wizard *wiz;
  Fl_Input *fg_exe_;
  inline void cb_fg_exe__i(Fl_Input*, void*);
  static void cb_fg_exe_(Fl_Input*, void*);
  inline void cb__i(Fl_Button*, void*);
  static void cb_(Fl_Button*, void*);
  Fl_Input *fg_root_;
  inline void cb_fg_root__i(Fl_Input*, void*);
  static void cb_fg_root_(Fl_Input*, void*);
  inline void cb_1_i(Fl_Button*, void*);
  static void cb_1(Fl_Button*, void*);
  Fl_Input *fg_scenery_;
  inline void cb_fg_scenery__i(Fl_Input*, void*);
  static void cb_fg_scenery_(Fl_Input*, void*);
  inline void cb_2_i(Fl_Button*, void*);
  static void cb_2(Fl_Button*, void*);
  Fl_Button *cache_delete;
  inline void cb_cache_delete_i(Fl_Button*, void*);
  static void cb_cache_delete(Fl_Button*, void*);
  Fl_Output *cache_file;
  Fl_Browser *aircraft;
  inline void cb_aircraft_i(Fl_Browser*, void*);
  static void cb_aircraft(Fl_Browser*, void*);
  Fl_Plib *preview;
  AirportBrowser *airports_;
  Fl_Output *text;
  inline void cb_Advanced_i(Fl_Button*, void*);
  static void cb_Advanced(Fl_Button*, void*);
public:
  Fl_Group *page[5];
private:
  Fl_Button *cancel;
  inline void cb_cancel_i(Fl_Button*, void*);
  static void cb_cancel(Fl_Button*, void*);
public:
  ~Wizard();
  void show();
  void show( int argc, char* argv[] );
  void init();
  void update_preview();
private:
  void preview_aircraft();
  void aircraft_update();
  void next_cb();
  void prev_cb();
  void advanced_options_cb();
  void fg_exe_update_cb();
  void fg_exe_select_cb();
  void fg_root_update_cb();
  void fg_root_select_cb();
  void fg_scenery_update_cb();
  void fg_scenery_select_cb();
  void advanced_cb();
  int write_fgfsrc();
  int write_fgfsrc( std::ostream&, const char* pfx = "\n");
  void run_fgfs();
  static void stdout_cb( int, void* );
  void stdout_cb( int );
  Fl_Preferences prefs;
  LogWindow* logwin;
  void cancel_cb();
  void delete_cache_file_cb();
  static void airports_cb( Fl_Widget*, void* );
  void airports_cb();
};
#endif
