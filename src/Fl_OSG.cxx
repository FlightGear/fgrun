#include <FL/Fl.H>

#include "Fl_OSG.h"
#include "i18n.h"
#include <cmath>

#include <osg/LineWidth>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#define HUD_SCALE_FACTOR    1.5

Fl_OSG::Fl_OSG( int x, int y, int w, int h, const char *label )
: AdapterWidget(x,y,w,h,label) {
    getCamera()->setViewport( new osg::Viewport( 0, 0, w, h ) );
    getCamera()->setProjectionMatrixAsPerspective( 30.0f, static_cast<double>(w)/static_cast<double>(h), 1.0f, 10000.0f );
    getCamera()->setGraphicsContext( getGraphicsWindow() );
    setThreadingModel( osgViewer::Viewer::SingleThreaded );

    hud = new osg::Camera;
    hud->setProjectionMatrix( osg::Matrix::ortho( 0, w*HUD_SCALE_FACTOR, 0, h*HUD_SCALE_FACTOR, 0, -1 ) );
    hud->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    hud->setViewMatrix( osg::Matrix::identity() );
    hud->setClearMask( GL_DEPTH_BUFFER_BIT );
    hud->setRenderOrder( osg::Camera::POST_RENDER );
    hud->setAllowEventFocus( false );

    setCameraManipulator( new osgGA::TrackballManipulator );
    addEventHandler( new osgViewer::StatsHandler );
    setSceneData( new osg::Node );
}

void Fl_OSG::draw() {
    frame();
}

static
osg::Geometry *drawStar( osg::Vec3 position, int xincr, double size, double depth, osg::Vec4 color ) {
    osg::Geometry* geom = new osg::Geometry;
    osg::Vec3Array* vertices = new osg::Vec3Array;
    osg::DrawElementsUByte *de = new osg::DrawElementsUByte( GL_TRIANGLES );

    osg::Vec3 delta( 110+xincr, 7, 0 );
    double innerScale = cos( 2.*M_PI / 5. ) / cos( M_PI / 5. );
    double dAngle = M_PI / 5.;
    for ( int i = 0; i < 5; ++i ) {
        double angle = (2. * i * M_PI / 5.) + (M_PI / 2.);
        vertices->push_back( position+delta+osg::Vec3( cos( angle )*size, sin( angle )*size, depth ) );
        vertices->push_back( position+delta+osg::Vec3( cos( angle+dAngle )*size*innerScale, sin( angle+dAngle )*size*innerScale, depth ) );
        de->push_back( (2*i + 9) % 10 );
        de->push_back( 2*i );
        de->push_back( 2*i + 1 );
    }
    geom->setVertexArray(vertices);
    de->push_back(1); de->push_back(3); de->push_back(5);
    de->push_back(5); de->push_back(7); de->push_back(9);
    de->push_back(1); de->push_back(5); de->push_back(9);

    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
    geom->setNormalArray(normals);
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(color);
    geom->setColorArray(colors);
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    osg::StateSet* stateset = geom->getOrCreateStateSet();
    stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    geom->addPrimitiveSet(de);
    return geom;
}

static
osg::Geometry *drawUnrated( osg::Vec3 position, int xincr ) {
    osg::Geometry* geom = new osg::Geometry;
    osg::Vec3Array* vertices = new osg::Vec3Array;
    osg::Vec3 delta( 110+xincr, 7, 0 );
    vertices->push_back(osg::Vec3(position.x()+100+xincr,position.y()-2,0));
    vertices->push_back(osg::Vec3(position.x()+225+xincr,position.y()+14,0));
    geom->setVertexArray(vertices);

    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
    geom->setNormalArray(normals);
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.,0.,0.,7.));
    geom->setColorArray(colors);
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);

    osg::StateSet* stateset = geom->getOrCreateStateSet();
    stateset->setAttributeAndModes( new osg::LineWidth(1) );

    geom->addPrimitiveSet(new osg::DrawArrays(GL_LINES,0,2));
    //geom->setEventCallback( 
    return geom;
}

static
void addBar( osg::Geode *geode, osg::Vec3 &position, int xincr, int val ) {
    for ( int i = 0; i < 5; ++i ) {
        geode->addDrawable( drawStar( position+osg::Vec3(25*i,0,0), xincr, 12, -0.1, osg::Vec4( 1., 1., .8, 0.2 ) ) );
    }

    for ( int i=0; i < val; ++i ) {
        geode->addDrawable( drawStar( position+osg::Vec3(25*i,0,0), xincr, 9, 0, osg::Vec4( 1., 1., 0., 0.7 ) ) );
    }
    if (val < 0)
        geode->addDrawable( drawUnrated( position, xincr ) );
}

static
osgText::Text *drawText( const char *t, osg::Vec3 position ) {
    osgText::Text* text = new osgText::Text;
    text->setFont("fonts/arial.ttf");
    text->setPosition(position);
    text->setCharacterSize( 20.0 );
    text->setText(t);
    return text;
}

static
void drawRating( osg::Geode *geode, osg::Vec3 position, const char *t, int rating, int xincr ) {
    geode->addDrawable( drawText( t, position ) );
    addBar( geode, position, xincr, ( rating >= 0 && rating <= 5 ) ? rating : -1 );
}

void Fl_OSG::set_model( osg::Node *m, int fdm, int systems, int cockpit, int model ) {
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild( m );

    hud->removeChildren( 0, hud->getNumChildren() );
    osg::Geode *geode = new osg::Geode;
    osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    osg::Vec3 position(30.0f,10.0f,0.0f);
    osg::Vec3 deltaY(0.0f,22.0f,0.0f);
    osg::Vec3 deltaX(20.0f,0.0f,0.0f);
    int xincr = atoi( _("RatingPos") );

    bool hasRating = false;
    if (model >= 0 && model <= 5 ) {
        drawRating( geode, position, _("Model : "), model, xincr );
        position += deltaY;
        hasRating = true;
    }
    if (cockpit >= 0 && cockpit <= 5 ) {
        drawRating( geode, position, _("Cockpit : "), cockpit, xincr );
        position += deltaY;
        hasRating = true;
    }
    if (systems >= 0 && systems <= 5 ) {
        drawRating( geode, position, _("Systems : "), systems, xincr );
        position += deltaY;
        hasRating = true;
    }
    if (fdm >= 0 && fdm <= 5 ) {
        drawRating( geode, position, _("FDM : "), fdm, xincr );
        position += deltaY;
        hasRating = true;
    }

    position -= deltaX;
    if (hasRating) {
        geode->addDrawable( drawText( _("Rating"), position ) );
    } else {
        geode->addDrawable( drawText( _("No Rating"), position ) );
    }
    position += deltaY;

    hud->addChild( geode );

    root->addChild( hud );
    setSceneData( root );
}

void Fl_OSG::resize(int x, int y, int w, int h) {
    hud->setProjectionMatrix( osg::Matrix::ortho( 0, w*HUD_SCALE_FACTOR, 0, h*HUD_SCALE_FACTOR, 0, -1 ) );
    AdapterWidget::resize(x, y, w, h);
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
