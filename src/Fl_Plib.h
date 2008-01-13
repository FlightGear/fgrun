#ifndef Fl_Plib_h
#define Fl_Plib_h

#include <FL/Fl_Gl_Window.H>
#include <string>

#include <osgViewer/Viewer>

class AdapterWidget : public Fl_Gl_Window {
public:

    AdapterWidget(int x, int y, int w, int h, const char *label=0);
    virtual ~AdapterWidget() {}

    osgViewer::GraphicsWindow* getGraphicsWindow() { return _gw.get(); }
    const osgViewer::GraphicsWindow* getGraphicsWindow() const { return _gw.get(); }

    virtual void resize(int x, int y, int w, int h);

protected:

    virtual int handle(int event);
    
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;
};

class Fl_Plib : public osgViewer::Viewer, public AdapterWidget {
public:
    Fl_Plib(int x, int y, int w, int h, const char *label=0);
    void set_model( osg::Node *model, osg::Node *bounding = 0 );
    void update();

protected:
    virtual void draw();
};

#endif
