#include <vector>

#include "Fl_Table_Row.H"

struct apt_dat_t;

/**
 * 
 */
class AirportBrowser : public Fl_Table_Row
{
public:
    typedef std::vector< const apt_dat_t* > airports_t;
    typedef airports_t::iterator iterator;
    typedef airports_t::const_iterator const_iterator;

public:
    AirportBrowser( int x, int y, int w, int h, const char *l=0 );
    ~AirportBrowser();

    void set_airports( const airports_t& apts );

    void select_id( const char* id );
    void select_name( const char* name );

protected:
    void draw_cell( TableContext context, int R=0, int C=0,
		    int X=0, int Y=0, int W=0, int H=0 );
    void sort_column( int col, bool reverse = false );

private:
    static void event_callback( Fl_Widget*, void* );
    void event_callback();

private:
    airports_t rowdata_;
    bool sort_reverse_;
    int sort_lastcol_;
};
