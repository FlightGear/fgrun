#include <FL/Fl.H>

#include "Fl_OSG.h"
#include "i18n.h"

#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

Fl_OSG::Fl_OSG( int x, int y, int w, int h, const char *label )
: AdapterWidget(x,y,w,h,label) {
    getCamera()->setViewport( new osg::Viewport( 0, 0, w, h ) );
    getCamera()->setProjectionMatrixAsPerspective( 30.0f, static_cast<double>(w)/static_cast<double>(h), 1.0f, 10000.0f );
    getCamera()->setGraphicsContext( getGraphicsWindow() );
    setThreadingModel( osgViewer::Viewer::SingleThreaded );

    hud = new osg::Camera;
    hud->setProjectionMatrix( osg::Matrix::ortho( 0, w, 0, h, 0, -1 ) );
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
    osg::Vec3 delta( 110+xincr, 7, 0 );
    vertices->push_back(position+delta+osg::Vec3( 0.951057,  0.309017, depth)*size);
    vertices->push_back(position+delta+osg::Vec3(-0.587785, -0.809017, depth)*size);
    vertices->push_back(position+delta+osg::Vec3(-0.000000, -0.381966, depth)*size);
    vertices->push_back(position+delta+osg::Vec3( 0.000000,  1.000000, depth)*size);
    vertices->push_back(position+delta+osg::Vec3(-0.224514,  0.309017, depth)*size);
    vertices->push_back(position+delta+osg::Vec3(-0.363271, -0.118034, depth)*size);
    vertices->push_back(position+delta+osg::Vec3( 0.587785, -0.809017, depth)*size);
    vertices->push_back(position+delta+osg::Vec3( 0.224514,  0.309017, depth)*size);
    vertices->push_back(position+delta+osg::Vec3( 0.363271, -0.118034, depth)*size);
    vertices->push_back(position+delta+osg::Vec3(-0.951056,  0.309017, depth)*size);
    geom->setVertexArray(vertices);

    osg::DrawElementsUByte *de = new osg::DrawElementsUByte( GL_TRIANGLES );
    de->push_back(3); de->push_back(4); de->push_back(7);
    de->push_back(0); de->push_back(7); de->push_back(8);
    de->push_back(2); de->push_back(6); de->push_back(8);
    de->push_back(1); de->push_back(2); de->push_back(5);
    de->push_back(4); de->push_back(9); de->push_back(5);
    de->push_back(4); de->push_back(5); de->push_back(7);
    de->push_back(5); de->push_back(8); de->push_back(7);
    de->push_back(2); de->push_back(8); de->push_back(5);

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
void addBar( osg::Geode *geode, osg::Vec3 &position, int xincr, int val ) {
    for ( int i = 0; i < 5; ++i ) {
        geode->addDrawable( drawStar( position+osg::Vec3(25*i,0,0), xincr, 12, -0.1, osg::Vec4( 1., 1., .8, 0.2 ) ) );
    }

    for ( int i=0; i < val; ++i ) {
        geode->addDrawable( drawStar( position+osg::Vec3(25*i,0,0), xincr, 9, 0, osg::Vec4( 1., 1., 0., 0.7 ) ) );
    }
}

void Fl_OSG::set_model( osg::Node *m, int fdm, int systems, int cockpit, int model ) {
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild( m );

    hud->removeChildren( 0, hud->getNumChildren() );
    osg::Geode *geode = new osg::Geode;
    std::string font("fonts/arial.ttf");
    osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    osg::Vec3 position(30.0f,10.0f,0.0f);
    osg::Vec3 deltaY(0.0f,22.0f,0.0f);
    osg::Vec3 deltaX(20.0f,0.0f,0.0f);
    int xincr = atoi( _("RatingPos") );
    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont(font);
        text->setPosition(position);
        text->setCharacterSize( 20.0 );

        if ( model >= 0 && model <= 5 ) {
            text->setText(_("Model : "));
            addBar( geode, position, xincr, model );
        }
        else {
            text->setText(_("Model : unrated"));
        }

        position += deltaY;
    }
    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont(font);
        text->setPosition(position);
        text->setCharacterSize( 20.0 );

        if ( cockpit >= 0 && cockpit <= 5 ) {
            text->setText(_("Cockpit : "));
            addBar( geode, position, xincr, cockpit );
        }
        else {
            text->setText(_("Cockpit : unrated"));
        }

        position += deltaY;
    }
    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont(font);
        text->setPosition(position);
        text->setCharacterSize( 20.0 );

        if ( systems >= 0 && systems <= 5 ) {
            text->setText(_("Systems : "));
            addBar( geode, position, xincr, systems );
        }
        else {
            text->setText(_("Systems : unrated"));
        }

        position += deltaY;
    }
    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont(font);
        text->setPosition(position);
        text->setCharacterSize( 20.0 );

        if ( fdm >= 0 && fdm <= 5 ) {
            text->setText(_("FDM : "));
            addBar( geode, position, xincr, fdm );
        }
        else {
            text->setText(_("FDM : unrated"));
        }

        position += deltaY;
        position -= deltaX;
    }
    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont(font);
        text->setPosition(position);
        text->setCharacterSize( 20.0 );
        text->setText(_("Rating"));
        position += deltaY;
    }
    hud->addChild( geode );

    root->addChild( hud );
    setSceneData( root );
}

void Fl_OSG::resize(int x, int y, int w, int h) {
    hud->setProjectionMatrix( osg::Matrix::ortho( 0, w, 0, h, 0, -1 ) );
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
