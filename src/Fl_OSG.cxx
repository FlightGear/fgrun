#include <FL/Fl.H>

#include "Fl_OSG.h"

#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

Fl_OSG::Fl_OSG( int x, int y, int w, int h, const char *label )
: AdapterWidget(x,y,w,h,label) {
    getCamera()->setViewport( new osg::Viewport( 0, 0, w, h ) );
    getCamera()->setProjectionMatrixAsPerspective( 30.0f, static_cast<double>(w)/static_cast<double>(h), 1.0f, 10000.0f );
    getCamera()->setGraphicsContext( getGraphicsWindow() );
    setThreadingModel( osgViewer::Viewer::SingleThreaded );

    setCameraManipulator( new osgGA::TrackballManipulator );
    addEventHandler( new osgViewer::StatsHandler );
    setSceneData( new osg::Node );
}

void Fl_OSG::draw() {
    frame();
}

void Fl_OSG::set_model( osg::Node *model, osg::Node *bounding ) {
    setSceneData( model );
}

void Fl_OSG::update() {
}

void idle_cb()
{
    Fl::redraw();
}

AdapterWidget::AdapterWidget( int x, int y, int w, int h, const char *label )
: Fl_Gl_Window(x, y, w, h, label ) {
    _gw = new osgViewer::GraphicsWindowEmbedded( x, y, w, h );
}

void AdapterWidget::resize(int x, int y, int w, int h) {
    _gw->getEventQueue()->windowResize( x, y, w, h );
    _gw->resized( x, y, w, h );
    Fl_Gl_Window::resize( x, y, w, h );
}

int AdapterWidget::handle(int event) {
    switch(event) {
        case FL_PUSH:
            _gw->getEventQueue()->mouseButtonPress(Fl::event_x(), Fl::event_y(), Fl::event_button());
            return 1;
        case FL_MOVE:
        case FL_DRAG:
            _gw->getEventQueue()->mouseMotion(Fl::event_x(), Fl::event_y());
            return 1;
        case FL_RELEASE:
            _gw->getEventQueue()->mouseButtonRelease(Fl::event_x(), Fl::event_y(), Fl::event_button());
            return 1;
        case FL_KEYDOWN:
            _gw->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol)Fl::event_key());
            return 1;
        case FL_KEYUP:
            _gw->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol)Fl::event_key());
            return 1;
        default:
            // pass other events to the base class
            return Fl_Gl_Window::handle(event);
    }
}
