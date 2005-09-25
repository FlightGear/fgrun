#ifndef Fl_Plib_h
#define Fl_Plib_h

#include <FL/Fl_Gl_Window.H>
#include <string>

#include <plib/ssg.h>

class Fl_Plib : public Fl_Gl_Window
{
public:

    Fl_Plib( int x, int y, int w, int h, const char *l=0 );

    void init();
    ssgEntity* load( const std::string& fname, const std::string& tpath );
    void set_model( ssgEntity* obj, ssgEntity* bounding_obj = 0 );
    void clear();

    void update();

private:

    void draw();
    int handle(int);
    void make_matrix( sgMat4 );

    int ConstrainEl();
    void reset();

private:

    ssgRoot* scene;
    int go;
    int downx, downy;   
    GLfloat downDist, downEl, downAz, downEx, downEy, downEz;
    GLfloat EyeDist, EyeAz, EyeEl, Ex, Ey, Ez;
    GLfloat EL_SENS, AZ_SENS;
};
#endif


