#ifndef Fl_OSG_h
#define Fl_OSG_h

#include <FL/Fl_Gl_Window.H>
#include <string>
#include <simgear/misc/sg_path.hxx>

#include <osgViewer/Viewer>

namespace osgText {
    class Text;
}

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

class Fl_OSG : public osgViewer::Viewer, public AdapterWidget {
public:
    Fl_OSG(int x, int y, int w, int h, const char *label=0);
    void set_model( osg::Node *m, int fdm, int systems, int cockpit, int model );
    void set_thumbnail( const char *path, int fdm, int systems, int cockpit, int model );
    void set_fg_root( const char *fgr );
    virtual void resize(int x, int y, int w, int h);

protected:
    virtual void draw();
    osgText::Text *drawText( const char *t, osg::Vec3 position );
    void drawRating( osg::Geode *geode, osg::Vec3 position, const char *t, int rating, int xincr );
    void drawRatings( osg::Group *root, int fdm, int systems, int cockpit, int model, const char *path = 0 );
    void setThumbnailTransform(int w, int h);

    osg::ref_ptr<osg::Camera> hud;
    osg::ref_ptr<osg::MatrixTransform> thumbnailTransform;
    float imgAspect;
    SGPath font;
};

#endif
