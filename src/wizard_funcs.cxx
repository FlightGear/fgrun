// wizard_funcs.cxx -- Wizard UI functions.
//
// Written by Bernie Bright, started Nov 2003.
//
// Copyright (c) 2003  Bernie Bright - bbright@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// $Id$

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <string>
#include <cstdio>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <sstream>
#include <string.h>
#include <cctype>
#include <sys/stat.h>
#include <errno.h>
#include <boost/algorithm/string/predicate.hpp>

#include <FL/Fl.H>
#include <FL/Fl_Preferences.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

#include <simgear/math/SGMath.hxx>
#include <simgear/props/props_io.hxx>
#include <simgear/structure/exception.hxx>
#include <simgear/misc/sg_path.hxx>
#include <simgear/misc/sg_dir.hxx>
#include <simgear/io/raw_socket.hxx>
#include <simgear/misc/ResourceManager.hxx>

#include <osg/MatrixTransform>
#include <osgDB/ReadFile>

#include "wizard.h"
#include "advanced.h"
#include "logwin.h"
#include "util.h"
#include "i18n.h"

#if defined(WIN32) || defined(__EMX__) && !defined(__CYGWIN__)
# include "os_win32.h"
# define strcasecmp stricmp
#else
# include "os_posix.h"
#endif

using std::string;
using std::vector;
using std::deque;
using std::set;
using std::map;

extern string def_fg_exe;
extern string def_fg_root;
extern string def_fg_scenery;

Fl_Menu_Item Wizard::menu_time_of_day_value[] = {
 {N_("dawn"), 0,  0, (void*)"dawn", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {N_("morning"), 0,  0, (void*)"morning", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {N_("noon"), 0,  0, (void*)"noon", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {N_("afternoon"), 0,  0, (void*)"afternoon", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {N_("dusk"), 0,  0, (void*)"dusk", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {N_("evening"), 0,  0, (void*)"evening", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {N_("midnight"), 0,  0, (void*)"midnight", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item Wizard::menu_season[] = {
 {N_("summer"), 0,  0, (void*)"summer", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {N_("winter"), 0,  0, (void*)"winter", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

// This array is not used in the code but is there to put possible status in the translation file
// Put here all possible variations
static const char *aircraft_status_[] = {
    N_("production"),
    N_("early-production"),
    N_("early production"),
    N_("Early production"),
    N_("beta"),
    N_("alpha"),
    N_("early alpha"),
    N_("Development"),
    N_("developement"),
    N_("development"),
    N_("devel"),
    N_("Early development"),
    N_("experimental"),
    0
};

/**
 * Data associated with each entry in the aircraft browser.
 */
struct AircraftData
{
    string name;
    string dir;
    string root;
    string desc;
    string status;
    string author;
    string modelPath;
    string thumbnailPath;
    int fdm;
    int systems;
    int cockpit;
    int model;
};

static int widths[] = { 465, 30, 0 };

static bool
is_valid_fg_exe( string exe )
{
    if (exe.size() == 0)
        return false;

    SGPath path( exe );
    while (exe.size() != 0 && ( !path.exists() || path.isDir() || boost::algorithm::ends_with(exe,"fgrun.exe") || boost::algorithm::ends_with(exe,"fgjs.exe")))
    {
        size_t pos = exe.rfind(' ');
        if (pos == std::string::npos)
            return false;
        else
            exe.erase( pos );
        path = SGPath( exe );
    }

    return true;
}

static bool
is_valid_fg_root( const string& dir )
{
    if (!fl_filename_isdir( dir.c_str() ))
        return false;

    string fname( dir );
    fname.append( "/version" );
    FILE* fp = fopen( fname.c_str(), "r" );
    if (fp != 0)
    {
        fclose(fp);
        return true;
    }

    return false;
}

void
Wizard::show_installed( bool state )
{
  if( state )
    airports_->show_installed(true);
  else
    airports_->show_installed(false);
}

void
Wizard::airports_cb( Fl_Widget*, void* v )
{
    ((Wizard*)v)->airports_cb();
}

void
Wizard::airports_cb()
{
    if (airports_->loaded())
    {
        airports_->show_installed( true );
        cache_delete_->activate();

        const int buflen = FL_PATH_MAX;
        char buf[ buflen ];

        if (prefs.get( "airport", buf, "", buflen-1) && buf[0] != 0)
            airports_->select_id( buf );
        if (prefs.get( "carrier", buf, "", buflen-1) && buf[0] == 0 &&
            prefs.get( "parkpos", buf, "", buflen-1) && buf[0] != 0)
        {
            airports_->select_parking( buf );
        }
        else if (prefs.get( "runway", buf, "", buflen-1) && buf[0] != 0)
            airports_->select_rwy( buf );
    }
    else
    {
        vector<string> v;
        for (int i = 1; i <= scenery_dir_list_->size(); ++i)
        {
            std::string path = scenery_dir_list_->text(i);
            std::string::size_type p;
            if ((p = path.find('\t')) != std::string::npos)
                path.erase( p );
            SGPath dir( path.c_str() );
            dir.append( "Terrain" );
            if (fl_filename_isdir( dir.c_str() ))
                v.push_back( dir.str() );
            else
                v.push_back( path.c_str() );
        }

        string cache( cache_file_->value() );
        airports_->load_airports( v, cache, airports_cb, this );
    }
}

static const char* about_text = N_("\
<html>\
 <head>\
  <title>FlightGear Launch Control %s</title>\
 </head>\
 <body>\
  <h1>FlightGear Launch Control for FlightGear v%s</h1>\
  <p>This program is released under the <a href=\"http://www.gnu.org/copyleft/gpl.html\">GNU General Public License</a>.</p>\
  <p>Report bugs to <a href=\"http://flightgear-bugs.googlecode.com\">http://flightgear-bugs.googlecode.com</a></p>\
 </body>\
</html>");

void
Wizard::reset()
{
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

    bool reloadPath = false;
    int version, systemVersion;
    prefs.get("version", version, 0);
    systemPrefs.get("version", systemVersion, 0);
    if (systemVersion != 0 && version < systemVersion)
    {
        prefs.set("version", systemVersion);
        reloadPath = true;
    }

    if (reloadPath || !prefs.get( "fg_exe", buf, def_fg_exe.c_str(), buflen-1))
    {
        systemPrefs.get( "fg_exe_init", buf, def_fg_exe.c_str(), buflen-1);
        prefs.set("fg_exe_init", buf);
        systemPrefs.get( "fg_exe", buf, def_fg_exe.c_str(), buflen-1);
        prefs.set("fg_exe", buf);
    }
    fg_exe_->value( buf );

    if (reloadPath || !prefs.get( "fg_root", buf, def_fg_root.c_str(), buflen-1))
    {
        systemPrefs.get( "fg_root_init", buf, def_fg_root.c_str(), buflen-1);
        prefs.set("fg_root_init", buf);
        systemPrefs.get( "fg_root", buf, def_fg_root.c_str(), buflen-1);
        prefs.set("fg_root", buf);
    }
    fg_root_->value( buf );
    SGPath fgPath(buf);
    if ( fg_root_->size() == 0 )
    {
        char *e = getenv( "FG_ROOT" );
        if ( e )
        {
            prefs.set( "fg_root", e );
            fg_root_->value( e );
            fgPath = e;
        }
    }

    simgear::ResourceManager::instance()->addBasePath(fgPath);

    string fg_aircraft;
    if (prefs.get( "fg_aircraft", buf, "", buflen-1))
    {
        fg_aircraft = buf;
    }

    aircraft_dir_list_->clear();
    typedef vector<string> vs_t;
    vs_t va( sgPathSplit( fg_aircraft ) );
    for (vs_t::size_type i = 0; i < va.size(); ++i)
        aircraft_dir_list_->add( va[i].c_str() );

    string fg_scenery;
    if (!reloadPath && prefs.get( "fg_scenery", buf, "", buflen-1))
    {
        fg_scenery = buf;
    }
    else if (reloadPath && systemPrefs.get( "fg_scenery", buf, "", buflen-1))
    {
        fg_scenery = buf;
        prefs.set("fg_scenery", buf);
        systemPrefs.get( "fg_scenery_init", buf, def_fg_exe.c_str(), buflen-1);
        prefs.set("fg_scenery_init", buf);
    }
    else if (!def_fg_scenery.empty())
    {
        fg_scenery = def_fg_scenery;
    }
    else if (fg_root_->size() > 0)
    {
        char *e = getenv( "FG_SCENERY" );
        if ( e )
        {
            prefs.set( "fg_scenery", e );
            fg_scenery = e;
        }
        else
        {
            fg_scenery = fg_root_->value();
            fg_scenery += "/Scenery";
        }
    }

    scenery_dir_list_->clear();
    vs_t vs( sgPathSplit( fg_scenery ) );

    int iVal;

    for (vs_t::size_type i = 0; i < vs.size(); ++i)
    {
        scenery_dir_list_->add( vs[i].c_str() );
    }

    bool fg_exe_ok = fg_exe_->size() != 0 && is_valid_fg_exe( fg_exe_->value() ),
         fg_root_ok = fg_root_->size() != 0 && is_valid_fg_root( fg_root_->value() );
    if ( !fg_exe_ok )
    {
        fg_exe_invalid->show();
    }
    if ( !fg_root_ok )
    {
        fg_root_invalid->show();
    }
    if (fg_scenery.empty())
    {
        fg_scenery_invalid->show();
    }
    if (!fg_exe_ok ||
        !fg_root_ok ||
        fg_scenery.empty() )
    {
        // First time through or FG_ROOT is not valid.
        page[0]->activate();
        page[0]->show();
        prev->deactivate();
        next->deactivate();
    }
    else
    {
        airports_->set_refresh_callback( refresh_airports, this );
        refresh_airports();
        aircraft_mru_update();
        aircraft_update();

        fg_exe_invalid->hide();
        fg_root_invalid->hide();
        prev->activate();
        next->activate();
        page[1]->show();
    }
    next->label( _("Next") );

    prefs.get("show_cmd_line", iVal, 0);
    show_cmd_line->value(iVal);
    if ( iVal )
        text->show();
    else
        text->hide();

    prefs.get("show_3d_preview", iVal, 1);
    show_3d_preview->value(iVal);
}

void
Wizard::init( bool fullscreen )
{
    for ( int i = 0; menu_time_of_day_value[i].text != 0; ++i )
    {
        menu_time_of_day_value[i].text = _( menu_time_of_day_value[i].text );
    }
    time_of_day_value->menu( menu_time_of_day_value );

    for ( int i = 0; menu_season[i].text != 0; ++i )
    {
        menu_season[i].text = _( menu_season[i].text );
    }
    season->menu( menu_season );

    scenery_dir_list_->column_widths(widths);
    scenery_dir_list_->column_char('\t');

    static const int npages = 5;

    make_launch_window();
    make_crash_window();

    for (int i = 0; i < npages; ++i)
        page[i]->hide();

    if (fullscreen)
    {
        win->resize( 0, 0, Fl::w(), Fl::h() );
    }
    else
    {
        int X, Y, W, H;
        prefs.get( "x", X, 0 );
        prefs.get( "y", Y, 0 );
        prefs.get( "width", W, 0 );
        prefs.get( "height", H, 0 );
        if (W > 0 && H > 0)
            win->resize( X, Y, W, H );
    }

    win->size_range( 800, 600 );
    text->buffer( new Fl_Text_Buffer );

    multiplay_callsign->maximum_size( 7 );

    logwin = new LogWindow( 640, 480, _("Log Window") );

    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

    prefs.getUserdataPath( buf, sizeof(buf) );
    SGPath cache( buf );
    cache.append( "airports.txt" );
    cache_file_->value( cache.c_str() );

    char *about_text_buffer = new char[2*strlen(about_text)];
#if defined( _MSC_VER ) && defined( _DEBUG )
    sprintf( about_text_buffer, about_text, VERSION, VERSION );
#else
    sprintf( about_text_buffer, _(about_text), VERSION, VERSION );
#endif
    about_->value( about_text_buffer );
    delete[] about_text_buffer;

    reset();
}

void
Wizard::show()
{
    win->show();
}

void
Wizard::show( int ac, char* av[] )
{
    win->show( ac, av );
}

static const double update_period = 0.05;

static void
timeout_handler( void* v )
{
    Fl::redraw();
    Fl::repeat_timeout( update_period, timeout_handler, v );
}

static SGPath
dir_path( const SGPath& p )
{
    return SGPath( p.dir() );
}

/**
 * Locate a named SSG node in a branch.
 */
static osg::Node *
find_named_node( osg::Node * node, const string &name )
{
    string node_name = node->getName();
    if ( name == node_name ) {
        return node;
    } else {
        osg::ref_ptr<osg::Group> group = node->asGroup();
        if ( group.valid() ) {
            int nKids = group->getNumChildren();
            for (int i = 0; i < nKids; i++) {
                osg::ref_ptr<osg::Node> result = find_named_node( group->getChild(i), name );
                if ( result.valid() )
                    return result.release();
            }
        }
    } 
    return 0;
}

osg::Node *
loadModel( const string &fg_root, const string &fg_aircraft, const string &dir, const string &current,
                const string &path, const SGPath& externalTexturePath )
{
    osg::ref_ptr<osg::Node> model;
    SGPropertyNode props;

    // Load the 3D aircraft object itself
    SGPath modelpath = path, texturepath = path;
    if ( modelpath.isRelative() )
    {
        SGPath tmp = current;
        tmp.append(modelpath.str());
        if (!tmp.exists())
        {
            tmp = fg_aircraft;
            tmp.append(modelpath.str());
        }
        if (!tmp.exists())
        {
            tmp = fg_aircraft;
            tmp.append(dir);
            tmp.append(modelpath.str());
        }
        if (!tmp.exists() && modelpath.str().find( "Aircraft/" ) == 0)
        {
            tmp = fg_aircraft;
            tmp.append(modelpath.str().substr(9));
        }
        if (!tmp.exists())
        {
            tmp = fg_root;
            tmp.append(modelpath.str());
        }
        modelpath = texturepath = tmp;
    }

    // Check for an XML wrapper
    if (modelpath.str().substr(modelpath.str().size() - 4, 4) == ".xml") {
        readProperties(modelpath.str(), &props);
        if (props.hasValue("/path")) {
            modelpath = modelpath.dir();
            string submodelpath = props.getStringValue("/path");
            modelpath.append(submodelpath);
            if (!modelpath.exists() && submodelpath.find( "Aircraft/" ) == 0)
            {
                modelpath = fg_aircraft;
                modelpath.append(submodelpath.substr(9));
            }
            if (!modelpath.exists())
            {
                modelpath = fg_root;
                modelpath.append(submodelpath);
            }
            if (props.hasValue("/texture-path")) {
                texturepath = texturepath.dir();
                string tpath = props.getStringValue("/texture-path");
                texturepath.append(tpath);
                if (!texturepath.exists() && tpath.find( "Aircraft/" ) == 0)
                {
                    texturepath = fg_aircraft;
                    texturepath.append(tpath.substr(9));
                }
                if (!texturepath.exists())
                {
                    texturepath = fg_root;
                    texturepath.append(tpath);
                }
            }
        } else if ( !model ) {
            model = new osg::Switch;
        }
    }

    vector<SGPropertyNode_ptr> nopreview_nodes = props.getChildren("nopreview");
    if ( !nopreview_nodes.empty() )
        return 0;

    osg::ref_ptr<osgDB::ReaderWriter::Options> options =
            new osgDB::ReaderWriter::Options(*osgDB::Registry::instance()->getOptions());

    // Assume that textures are in
    // the same location as the XML file.
    if (!model) {
        if (texturepath.extension() != "")
            texturepath = texturepath.dir();

        options->setDatabasePath(texturepath.str());
        if (!externalTexturePath.str().empty())
            options->getDatabasePathList().push_back(externalTexturePath.str());

        model = osgDB::readNodeFile(modelpath.str(), options.get());
        if ( model == 0 )
            throw sg_io_exception("Failed to load 3D model", 
                                sg_location(modelpath.str()));
    }

    // Set up the alignment node
    osg::ref_ptr<osg::MatrixTransform> alignmainmodel = new osg::MatrixTransform;
    alignmainmodel->addChild(model.get());
    osg::Matrix res_matrix;
    double  pitch = props.getDoubleValue("/offsets/pitch-deg", 0.0),
            roll = props.getDoubleValue("/offsets/roll-deg", 0.0),
            heading = props.getDoubleValue("/offsets/heading-deg", 0.0);
    res_matrix.makeRotate(
            pitch*SG_DEGREES_TO_RADIANS, osg::Vec3(0, 1, 0),
            roll*SG_DEGREES_TO_RADIANS, osg::Vec3(1, 0, 0),
            heading*SG_DEGREES_TO_RADIANS, osg::Vec3(0, 0, 1)
        );

    osg::Matrix tmat;
    tmat.makeTranslate(props.getFloatValue("/offsets/x-m", 0.0),
                    props.getFloatValue("/offsets/y-m", 0.0),
                    props.getFloatValue("/offsets/z-m", 0.0));
    alignmainmodel->setMatrix(res_matrix*tmat);

    // Load sub-models
    vector<SGPropertyNode_ptr> model_nodes = props.getChildren("model");
    unsigned int i = 0;
    for (i = 0; i < model_nodes.size(); i++) {
        SGPropertyNode_ptr node = model_nodes[i];
        string submodel = node->getStringValue("path");

        osg::ref_ptr<osg::Node> kid;
        try {
            kid = loadModel( fg_root, fg_aircraft, dir, modelpath.dir(), submodel, externalTexturePath );
        } catch (const sg_throwable &t) {
            SG_LOG(SG_INPUT, SG_ALERT, "Failed to load submodel: " << t.getFormattedMessage());
            throw;
        }

        osg::ref_ptr<osg::MatrixTransform> align = new osg::MatrixTransform;
        res_matrix.makeIdentity();
        pitch = node->getDoubleValue("offsets/pitch-deg", 0.0);
        roll = node->getDoubleValue("offsets/roll-deg", 0.0);
        heading = node->getDoubleValue("offsets/heading-deg", 0.0);
        res_matrix.makeRotate(
                pitch*SG_DEGREES_TO_RADIANS, osg::Vec3(0, 1, 0),
                roll*SG_DEGREES_TO_RADIANS, osg::Vec3(1, 0, 0),
                heading*SG_DEGREES_TO_RADIANS, osg::Vec3(0, 0, 1)
            );

        tmat.makeIdentity();
        tmat.makeTranslate(node->getDoubleValue("offsets/x-m", 0),
                        node->getDoubleValue("offsets/y-m", 0),
                        node->getDoubleValue("offsets/z-m", 0));
        align->setMatrix( res_matrix * tmat );
        align->addChild( kid.get() );

        align->setName( node->getStringValue( "name", "" ) );
        alignmainmodel->addChild( align.get() );
    }

    vector<SGPropertyNode_ptr> animation_nodes = props.getChildren("animation");
    for (i = 0; i < animation_nodes.size(); i++) {
        const char * name = animation_nodes[i]->getStringValue("name", 0);
        vector<SGPropertyNode_ptr> nopreview_nodes = animation_nodes[i]->getChildren("nopreview");
        if ( !nopreview_nodes.empty() ) {
            vector<SGPropertyNode_ptr> name_nodes = animation_nodes[i]->getChildren("object-name");
            for (size_t j = 0; j < name_nodes.size(); j++ ) {
                osg::Node *e = find_named_node( model.get(), name_nodes[j]->getStringValue() );
                if ( !e )
                    continue;
                osg::Group *b = e->getParent( 0 );
                b->removeChild( e );
            }
        }
    }

    return alignmainmodel.release();
}

void
Wizard::preview_aircraft(bool desel_mru)
{
    Fl::remove_timeout( timeout_handler, this );

    current_aircraft_model_path = "";

    preview->set_fg_root(fg_root_->value());
    preview->make_current();

    preview->clear();
    preview->redraw();

    int n = aircraft->value();
    if (n == 0)
        return;

    AircraftData* data =
        reinterpret_cast<AircraftData*>( aircraft->data(n) );
    prefs.set( "aircraft", n > 0 ? data->name.c_str() : "" );
    prefs.set( "aircraft_name", n > 0 ? data->desc.c_str() : "" );
    if (desel_mru)
        aircraft_mru->deselect();

    if (!data->modelPath.empty())
    {
        SGPath path( data->root ), tpath;
        path.append( data->modelPath );

        bool error = false;
        if (!path.exists())
        {
            error = true;
            path = data->root;
            path.append( data->dir );
            path.append( data->modelPath );

            if ( path.exists() )
                error = false;
            else if ( data->modelPath.find( "Aircraft/" ) == 0 )
            {
                path = data->root;
                path.append( data->modelPath.substr( 9 ) );
                error = !path.exists();
            }
        }
        if ( error )
        {
            fl_alert( _("Model not found: '%s'"), path.c_str() );
            return;
        }

        setlocale( LC_ALL, "C" );
        try
        {
            win->cursor( FL_CURSOR_WAIT );
            Fl::flush();

            if (show_3d_preview->value())
            {
                osg::ref_ptr<osg::Node> model = loadModel( fg_root_->value(), data->root, data->dir, "", path.str(), SGPath() );
                if (model != 0)
                {
                    current_aircraft_model_path = path.str();
                    //osg::ref_ptr<osg::Node> bounding_obj = find_named_node( model.get(), "Aircraft" );
                    preview->set_model( model.get(), data->fdm, data->systems, data->cockpit, data->model );

                    Fl::add_timeout( update_period, timeout_handler, this );
                }
            }
            else
            {
                string thumbnail = data->thumbnailPath;
                preview->set_thumbnail(thumbnail.c_str(), data->fdm, data->systems, data->cockpit, data->model );
            }
            win->cursor( FL_CURSOR_DEFAULT );
            preview->redraw();
        }
        catch (const sg_exception& exc )
        {
            fl_alert( "%s",exc.getFormattedMessage().c_str() );
            return;
        }
        catch (...)
        {}
        setlocale( LC_ALL, "" );
    }
    else
    {
        fl_alert( _("Property '/sim/model/path' not found") );
        return;
    }

    aircraft_status->value( _( data->status.c_str() ) );
    aircraft_author->value( data->author.c_str() );
    aircraft_location->value( data->root.c_str() );

    next->activate();
}

void
Wizard::next_cb()
{
    prev->activate();

    if (wiz->value() == page[0])
    {
        char abs_name[ FL_PATH_MAX ];

        fl_filename_absolute( abs_name, fg_exe_->value() );
        prefs.set( "fg_exe", abs_name );

        fl_filename_absolute( abs_name, fg_root_->value() );

        const int buflen = FL_PATH_MAX;
        char buf[ buflen ];
        const char* not_set = "NOT SET";
        prefs.get( "fg_root", buf, not_set, buflen-1);
        if ( strcmp( buf, abs_name ) != 0 )
        {
            prefs.set( "fg_root", abs_name );
             win->cursor( FL_CURSOR_WAIT );
            Fl::flush();
            aircraft_mru_update();
            aircraft_update();
            win->cursor( FL_CURSOR_DEFAULT );
        }

        string fg_aircraft;
        if ( aircraft_dir_list_->size() > 0 )
        {
            fg_aircraft = aircraft_dir_list_->text(1);
            for (int i = 2; i <= aircraft_dir_list_->size(); ++i)
            {
                fg_aircraft += os::searchPathSep;
                fg_aircraft += aircraft_dir_list_->text(i);
            }
        }
        prefs.set( "fg_aircraft", fg_aircraft.c_str() );

        string fg_scenery;
        for (int i = 1; i <= scenery_dir_list_->size(); ++i)
        {
            if (i != 1) fg_scenery += os::searchPathSep;
            std::string path = scenery_dir_list_->text(i);
            std::string::size_type p;
            if ((p = path.find('\t')) != std::string::npos)
                path.erase( p );
            fg_scenery += path;
        }
        prefs.set( "fg_scenery", fg_scenery.c_str() );

        if (refreshAircraft)
            aircraft_update();

        refresh_airports();
    }
    else if (wiz->value() == page[1])
    {
        int n = aircraft->value();
        if (n > 0)
        {
            AircraftData* data =
                reinterpret_cast<AircraftData*>( aircraft->data(n) );
            prefs.set( "aircraft", n > 0 ? data->name.c_str() : "" );
        }
        Fl::remove_timeout( timeout_handler, this );
    }
    else if (wiz->value() == page[2])
    {
        prefs.set( "airport", airports_->get_selected_id().c_str() );
        prefs.set( "airport-name",
                   airports_->get_selected_name().c_str() );
        prefs.set( "carrier", carrier_->value() );
        if ( carrier_->value() != string() )
            prefs.set( "parkpos", parkpos_->value() );
        else
            prefs.set( "parkpos", airports_->get_selected_parking().c_str() );

        string rwy( airports_->get_selected_runway() );
        if (rwy.empty())
            rwy = _("<default>");
        prefs.set( "runway", rwy.c_str() );

        update_basic_options( prefs );
        display_scenarii();
    }
    else if (wiz->value() == page[3])
    {
        update_aircraft_mru();

        prefs.flush();
        if (fgThread == 0)
            fgThread = new FlightGearThread( this );
        fgThread->setViewer(false);
        fgThread->start();

        exec_launch_window();
        return;
    }

    wiz->next();

    if (wiz->value() == page[1])
    {
        aircraft_mru_update();

        // "Select aircraft" page
        if (aircraft->size() == 0)
            aircraft_update();
    }
    else if (wiz->value() == page[2])
    {
        char buf[256];
        prefs.get( "carrier", buf, "", sizeof buf - 1 );
        carrier_->value( buf );
        prefs.get( "parkpos", buf, "", sizeof buf - 1 );
        if ( carrier_->value() != string() )
            parkpos_->value( buf );
        else
            airports_->select_parking( buf );

        // "Select location" page
        if (!airports_->loaded())
        {
             win->cursor( FL_CURSOR_WAIT );
            Fl::flush();
        }
    }
    else if (wiz->value() == page[3])
    {
        std::ostringstream ostr;
        ostr << fg_exe_->value() << "\n  ";
        write_fgfsrc( prefs, ostr, "\n  " );
        text->buffer()->text( ostr.str().c_str() );
        next->label( _("Run") );
    }
}

void
Wizard::prev_cb()
{
    next->activate();
    next->label( _("Next") );
    if (wiz->value() == page[2])
    {
        aircraft_mru_update();
    }
    wiz->prev();
    if (wiz->value() == page[0])
    {
        prev->deactivate();
    }
    else if (wiz->value() == page[1])
    {
        Fl::add_timeout( update_period, timeout_handler, this );
    }
}

void
Wizard::defaults_cb()
{
    int r = fl_choice( _("About to reset current parameters"), _("Abort"), _("Reset"), 0 );
    if (!r)
        return;
    if (adv == 0)
    {
        adv = new Advanced( prefs );
    }
    adv->reset_settings( prefs );
    reset_settings();
}

void
Wizard::fg_exe_select_cb()
{
    SGPath exe(fg_exe_->value());
    char* p = fl_file_chooser( _("Select FlightGear executable"), exe.file().c_str(), exe.dir().c_str());
    if (p != 0)
        fg_exe_->value( p );

    fg_exe_update_cb();
}

void
Wizard::fg_exe_update_cb()
{
    fg_path_updated();
}

void
Wizard::fg_root_update_cb()
{
    fg_path_updated();
}

void
Wizard::fg_path_updated(bool addScenery)
{
    next->deactivate();

    bool error = false;

    fg_exe_invalid->show();
    if (fg_exe_->size() == 0 || !is_valid_fg_exe(fg_exe_->value()))
        error = true;
    else
        fg_exe_invalid->hide();

    fg_root_invalid->show();
    string dir( fg_root_->value() );

    if (fg_root_->size() == 0)
        error = true;
    else
    {
        // Remove trailing separator.
        if (os::isdirsep( dir[ dir.length() - 1 ] ))
            dir.erase( dir.length() - 1 );

        if (!is_valid_fg_root( dir ))
        {
            dir.append( "/data" );
            if (!is_valid_fg_root( dir ))
                error = true;
            else
            {
                fg_root_->value( dir.c_str() );
                fg_root_invalid->hide();
            }
        }
        else
            fg_root_invalid->hide();
    }

    if (addScenery && scenery_dir_list_->size() == 0)
    {
        // Derive FG_SCENERY from FG_ROOT. 
        string d( dir );
        d.append( "/Scenery" );
        if (fl_filename_isdir( d.c_str() ))
            scenery_dir_list_->add( d.c_str() );
    }

    fg_scenery_invalid->show();
    if (scenery_dir_list_->size() == 0)
        error = true;
    else
        fg_scenery_invalid->hide();

    if (!error)
        next->activate();
}

void
Wizard::fg_root_select_cb()
{
    char* p = fl_dir_chooser( _("Select FG_ROOT directory"),
                              fg_root_->value(), 0);
    if (p != 0)
    {
        // Remove trailing separator.
        if (*p != 0 && os::isdirsep( p[strlen(p)-1] ))
            p[strlen(p)-1] = '\0';
        fg_root_->value( p );
    }

    fg_root_update_cb();
}

void
Wizard::advanced_cb()
{
    if (adv == 0)
        adv = new Advanced( prefs );

    prefs.set( "airport", airports_->get_selected_id().c_str() );
    prefs.set( "airport-name", airports_->get_selected_name().c_str() );

    int r = adv->exec( prefs );
    if (r)
    {
    }

    update_basic_options( prefs );

    // Update command text.
    std::ostringstream ostr;
    ostr << fg_exe_->value() << "\n  ";
    write_fgfsrc( prefs, ostr, "\n  " );
    text->buffer()->text( ostr.str().c_str() );
}

static void
search_aircraft_dir( const SGPath& dir,
                     bool recursive,
                     vector<SGPath>& ac )
{
    string s( dir.str() );

#ifdef WIN32
    // Ensure there is a trailing slash.
    if (*s.rbegin() != '/')
        s.append( "/" );
#endif

    simgear::Dir directory( dir );
    simgear::PathList files = directory.children();
    for ( simgear::PathList::iterator ii = files.begin(); ii != files.end(); ++ii )
    {
        if (fl_filename_match(ii->c_str(), "*-set.xml"))
        {
            ac.push_back( *ii );
        }
        else if ( recursive && ii->isDir() )
        {
            search_aircraft_dir( *ii, false, ac );
        }
    }
}

static void
delayed_preview( void* v )
{
    ((Wizard*)v)->preview_aircraft();
}

struct ICompare {
    bool operator()( const string &a, const string &b ) const {
        return strcasecmp( a.c_str(), b.c_str() ) < 0;
    }
};

void
Wizard::aircraft_update()
{
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];
    prefs.get( "aircraft", buf, "", buflen-1);
    aircraft_update( buf );
}

void
Wizard::aircraft_update( const char *aft )
{
    SGPath rpath( fg_root_->value() );
    rpath.append( "Aircraft" );
    map<string, vector< SGPath > > ac;
    search_aircraft_dir( rpath, true, ac[ rpath.str() ] );

    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];
    string fg_aircraft;
    if (prefs.get( "fg_aircraft", buf, "", buflen-1))
    {
        fg_aircraft = buf;
    }

    typedef vector<string> vs_t;
    vs_t va( sgPathSplit( fg_aircraft ) );
    for (vs_t::size_type i = 0; i < va.size(); ++i)
    {
        SGPath path( va[ i ] );
        search_aircraft_dir( path, true, ac[ path.str() ] );
    }

    // Empty the aircraft browser list.
    for (int i = 1; i <= aircraft->size(); ++i)
    {
        if ( aircraft->text(i)[0] == ' ' )
        {
            AircraftData* data =
                reinterpret_cast<AircraftData*>( aircraft->data(i) );
            delete data;
        }
    }
    aircraft->clear();

    bool selected = false;
    map<string,vector<AircraftData*>,ICompare> am;
    for ( map<string, vector< SGPath > >::iterator ii = ac.begin(); ii != ac.end(); ++ii )
    {
        // Populate the aircraft browser list.
        for (vector<SGPath>::size_type vi = 0; vi < ii->second.size(); ++vi)
        {
            SGPath path = ii->first;
            string s( ii->second[vi].str() ), name( s );
            name.erase( 0, path.str().size() );
            if ( name[0] == '/' )
                name.erase( 0, 1 );
            string::size_type p = name.find( '/' );
            if ( p != string::npos )
                name.erase( p );

            SGPropertyNode props;
            try
            {
                readProperties( s.c_str(), &props );
            }
            catch (const sg_exception&)
            {
                continue;
            }

            if (props.hasValue( "/sim/description" ))
            {
                string desc = props.getStringValue( "/sim/description" );

                if ( desc.find( "Alias " ) == string::npos )
                {
                    // Extract aircraft name from filename.
                    string::size_type pos = s.rfind( "/" );
                    string::size_type epos = s.find( "-set.xml", pos );
                    string ss( s.substr( pos+1, epos-pos-1 ) );

                    AircraftData* data = new AircraftData;
                    data->name = ss;
                    data->dir = name;
                    data->root = path.str();
                    data->desc = desc;
                    data->status = props.getStringValue( "/sim/status", _( "Unknown" ) );
                    data->modelPath = props.getStringValue( "/sim/model/path", _( "Unknown" ) );
                    string splash = props.getStringValue( "/sim/startup/splash-texture", _( "" ) );
                    if (splash.empty())
                    {
                        data->thumbnailPath = SGPath(s).dir() + "/thumbnail.jpg";
                    }
                    else
                    {
                        data->thumbnailPath = path.str().substr(0, path.str().size()-8) + splash;
                    }
                    data->author = props.getStringValue( "/sim/author", _( "Unknown" ) );
                    data->fdm = props.getIntValue( "/sim/rating/FDM", -1 );
                    data->systems = props.getIntValue( "/sim/rating/systems", -1 );
                    data->cockpit = props.getIntValue( "/sim/rating/cockpit", -1 );
                    data->model = props.getIntValue( "/sim/rating/model", -1 );
                    am[name].push_back( data );
                }
            }
        }
    }

    for ( map<string,vector<AircraftData*>,ICompare>::iterator it = am.begin(); it != am.end(); ++it )
    {
        aircraft->add( it->first.c_str(), it->second[0] );

        for ( vector<AircraftData*>::size_type i = 0; i < it->second.size(); ++i )
        {
            AircraftData* data = it->second[i];
            string desc = data->desc;
            desc.insert( 0, "    " );
            aircraft->add( desc.c_str(), data );

            if (aft[0] != 0 && strcmp( aft, data->name.c_str() ) == 0)
            {
                aircraft->select( aircraft->size() );
                selected = true;
            }
        }
    }

    refreshAircraft = false;

    if ( selected )
        Fl::add_timeout( 0.1, delayed_preview, this );
}

Wizard::~Wizard()
{
    // Empty the aircraft browser list.
    for (int i = 1; i <= aircraft->size(); ++i)
    {
        if ( aircraft->text(i)[0] == ' ' )
        {
            AircraftData* data =
                reinterpret_cast<AircraftData*>( aircraft->data(i) );
            delete data;
        }
    }
    aircraft->clear();

    delete logwin;
    delete win;
    delete adv;
}

void
Wizard::cancel_cb()
{
    stopProcess( tsPid );

    logwin->hide();

    // Save main window size and position.
    prefs.set( "x", win->x() );
    prefs.set( "y", win->y() );
    prefs.set( "width", win->w() );
    prefs.set( "height", win->h() );

    win->hide();
}

void
Wizard::delete_cache_file_cb()
{
    char buf[ FL_PATH_MAX ];
    prefs.getUserdataPath( buf, sizeof(buf) );
    SGPath path( buf );
    path.append( "/airports.txt" );

    if (!path.exists())
        return;

    //TODO: win32 support.
    if (unlink( path.c_str() ) == 0)
        return;

    fl_alert( _("Unable to delete '%s':\n%s"),
              path.c_str(), strerror(errno) );
}

void
Wizard::aircraft_dir_select_cb()
{
    int n = aircraft_dir_list_->value();
    if (n > 0)
    {
        aircraft_dir_delete_->activate();

        aircraft_dir_list_->deselect();
        aircraft_dir_list_->select( n );
    }
    else
    {
        aircraft_dir_delete_->deactivate();
    }
}

void
Wizard::aircraft_dir_add_cb()
{
    char* p = fl_dir_chooser( _("Select FG_AIRCRAFT directory"), 0, 0);
    if (p != 0)
    {
        // Remove trailing separator.
        if (*p != 0 && os::isdirsep( p[strlen(p)-1] ))
            p[strlen(p)-1] = '\0';
        aircraft_dir_list_->add( p );
        aircraft_dir_list_->value( aircraft_dir_list_->size() );
        aircraft_dir_list_->select( aircraft_dir_list_->size() );
        aircraft_dir_delete_->activate();

        refreshAircraft = true;
    }
}

void
Wizard::aircraft_dir_delete_cb()
{
    int n = aircraft_dir_list_->value();
    if (n > 0)
    {
        aircraft_dir_list_->remove( n );
    }

    if (aircraft_dir_list_->value() == 0)
        aircraft_dir_delete_->deactivate();

    refreshAircraft = true;
}

void
Wizard::scenery_dir_select_cb()
{
    scenery_dir_up_->deactivate();
    scenery_dir_down_->deactivate();
    int n = scenery_dir_list_->value();
    if (n > 0)
    {
        scenery_dir_delete_->activate();
        if (n > 1)
            scenery_dir_up_->activate();
        if (n < scenery_dir_list_->size())
            scenery_dir_down_->activate();

        scenery_dir_list_->deselect();
        scenery_dir_list_->select( n );
    }
    else
    {
        scenery_dir_delete_->deactivate();
    }
}

void
Wizard::update_scenery_up_down()
{
    int n = scenery_dir_list_->value();
    if (n <= 0)
        scenery_dir_up_->deactivate();
    else
        scenery_dir_up_->activate();
    if (n == 0 || n == scenery_dir_list_->size())
        scenery_dir_down_->deactivate();
    else
        scenery_dir_down_->activate();
    if (n == 0)
        scenery_dir_delete_->deactivate();
    else
        scenery_dir_delete_->activate();
}

void
Wizard::scenery_dir_add_cb()
{
    char* p = fl_dir_chooser( _("Select FG_SCENERY directory"), 0, 0);
    if (p != 0)
    {
        // Remove trailing separator.
        if (*p != 0 && os::isdirsep( p[strlen(p)-1] ))
            p[strlen(p)-1] = '\0';
        scenery_dir_list_->add( p );
        scenery_dir_list_->value( scenery_dir_list_->size() );
        scenery_dir_list_->select( scenery_dir_list_->size() );
        update_scenery_up_down();

        fg_path_updated();
    }
}

void
Wizard::scenery_dir_delete_cb()
{
    int n = scenery_dir_list_->value();
    if (n > 0)
    {
        scenery_dir_list_->remove( n );
    }

    if (scenery_dir_list_->size() == 0)
        scenery_dir_delete_->deactivate();

    update_scenery_up_down();

    fg_path_updated( false );
}

/**
 * Move a directory entry up the list.
 */
void
Wizard::scenery_dir_up_cb()
{
    int from = scenery_dir_list_->value();
    int to = from - 1;
    scenery_dir_list_->move( to, from );

    scenery_dir_list_->deselect();
    scenery_dir_list_->select( to );

    scenery_dir_down_->activate();
    if (to == 1)
        scenery_dir_up_->deactivate();
}

/**
 * Move a directory entry down the list.
 */
void
Wizard::scenery_dir_down_cb()
{
    int n = scenery_dir_list_->value();
    scenery_dir_list_->insert( n+2, scenery_dir_list_->text(n) );
    scenery_dir_list_->remove( n );

    scenery_dir_list_->deselect();
    scenery_dir_list_->select( n+1 );

    scenery_dir_up_->activate();
    if (n+1 == scenery_dir_list_->size())
        scenery_dir_down_->deactivate();
}

/**
 * Force a re-scan of the scenery directories for new airports.
 */
void
Wizard::refresh_airports( Fl_Widget*, void* v )
{
    static_cast<Wizard*>(v)->delete_cache_file_cb();
    static_cast<Wizard*>(v)->refresh_airports();
}


void
Wizard::refresh_airports()
{
//     win->cursor( FL_CURSOR_WAIT );

    SGPath path( fg_root_->value() );
    airports_->load_runways( path.str(), airports_cb, this );
}

void
Wizard::update_options()
{
    // Update command text.
    std::ostringstream ostr;
    ostr << fg_exe_->value() << "\n  ";
    write_fgfsrc( prefs, ostr, "\n  " );
    text->buffer()->text( ostr.str().c_str() );
}

void
Wizard::resolution_cb()
{
    prefs.set("geometry", resolution->text());
    update_options();
}

void
Wizard::bpp_cb()
{
    prefs.set("bpp", bpp->text());
    update_options();
}

void
Wizard::game_mode_cb()
{
    prefs.set("fullscreen", game_mode->value());
    update_options();
}

void
Wizard::horizon_effect_cb()
{
    prefs.set("horizon_effect", horizon_effect->value());
    update_options();
}

void
Wizard::enhanced_lighting_cb()
{
    prefs.set("enhanced_lighting", enhanced_lighting->value());
    update_options();
}

void
Wizard::specular_highlight_cb()
{
    prefs.set("specular_highlight", specular_highlight->value());
    update_options();
}

void
Wizard::clouds_3d_cb()
{
    prefs.set("clouds3d", clouds_3d->value());
    update_options();
}

void
Wizard::frame_rate_limiter_cb()
{
    int v = frame_rate_limiter->value();
    if ( v == 0 )
    {
        frame_rate_limiter_value->deactivate();
    }
    else
    {
        frame_rate_limiter_value->activate();
    }
    prefs.set( "frame_rate_limiter", v );
    frame_rate_limiter_value_cb();
}

void
Wizard::frame_rate_limiter_value_cb()
{
    prefs.set( "frame_rate_limiter_value", frame_rate_limiter_value->value() );
    update_options();
}

void
Wizard::random_objects_cb()
{
    prefs.set("random_objects", random_objects->value());
    update_options();
}

void
Wizard::random_trees_cb()
{
    prefs.set("random_trees", random_trees->value());
    update_options();
}

void
Wizard::ai_models_cb()
{
    prefs.set("ai_models", ai_models->value());
    if ( ai_models->value() == 0 )
    {
        ai_traffic->value(0);
        prefs.set("ai_traffic", ai_traffic->value());
        ai_traffic->deactivate();
        multiplay->value(0);
        multiplay_cb();
    }
    else
    {
        ai_traffic->activate();
        update_options();
    }
}

void
Wizard::ai_traffic_cb()
{
    prefs.set("ai_traffic", ai_traffic->value());
    update_options();
}

void
Wizard::time_of_day_cb()
{
    int v = time_of_day->value();
    if ( v == 0 )
    {
        time_of_day_value->deactivate();
        prefs.set("time-match-real", 1);
    }
    else
    {
        time_of_day_value->activate();
        prefs.set("time_of_day_value", (const char *)time_of_day_value->mvalue()->user_data_);
        prefs.set("time-match-real", 0);
        prefs.set("time-match-local", 0);
        prefs.set("start-date-sys", 0);
        prefs.set("start-date-gmt", 0);
        prefs.set("start-date-lat", 0);
    }
    prefs.set("time_of_day", v);
    update_options();
}

void
Wizard::time_of_day_value_cb()
{
    prefs.set("time_of_day_value", (const char *)time_of_day_value->mvalue()->user_data_);
    update_options();
}

void
Wizard::season_cb()
{
    prefs.set("season", (const char *)season->mvalue()->user_data_);
    update_options();
}

void
Wizard::real_weather_fetch_cb()
{
    prefs.set("fetch_real_weather", real_weather_fetch->value());
    update_options();
}

void
Wizard::auto_coordination_cb()
{
    prefs.set("auto_coordination", auto_coordination->value());
    update_options();
}

void
Wizard::scenarii_cb()
{
    static string tooltip;
    if ( scenarii->value() )
    {
        SGPath path( fg_root_->value() );
        path.append( "AI" );
        path.append( scenarii->text( scenarii->value() ) );
        path.concat( ".xml" );

        try
        {
            SGPropertyNode scenario;
            readProperties( path.str(), &scenario );

            tooltip = _("Description of ");
            tooltip += scenarii->text( scenarii->value() );
            tooltip += "\n";
            tooltip += scenario.getStringValue( "scenario/description", _("Not set") );

            size_t p = 0;
            while ( ( p = tooltip.find( '@', p ) ) != string::npos )
            {
                tooltip.insert( p, "@" );
                p += 2;
            }
        }
        catch ( const sg_exception& )
        {
            tooltip = _("Select a scenario to display its description");
        }
    }
    else
        tooltip = _("Select a scenario to display its description");

    scenarii_help->tooltip( tooltip.c_str() );

    int nb = 0;
    for (int i = 1; i <= scenarii->size(); ++i)
    {
        if ( scenarii->selected(i) )
        {
            nb += 1;
            prefs.set( Fl_Preferences::Name("scenario-item-%d", nb), scenarii->text(i));
        }
    }
    prefs.set("scenario-count", nb);
    update_options();
}

void
Wizard::terrasync_cb()
{
    int v = terrasync->value();
    if ( v == 0 )
    {
        prefs.set("terrasync",0);
    }
    else
    {
        prefs.set("terrasync",1);
    }
    update_options();
}

void
Wizard::atlas_cb()
{
    int v = atlas->value();
    if ( v == 0 )
    {
        atlas_host->deactivate();
        atlas_port->deactivate();
        std::vector<string> io_list;
        int i, iVal;
        prefs.get("io-count", iVal, 0);
        for ( i = 1; i <= iVal; ++i )
        {
            char buf[256];
            prefs.get( Fl_Preferences::Name( "io-item-%d", i ), buf, "", sizeof buf - 1 );
            string item( buf );
            if ( item.length() < 8 || item.substr( 0, 8 ) != "--atlas=" )
            {
                io_list.push_back(buf);
            }
        }
        prefs.set("io-count",(int)io_list.size());
        for ( i = 0; i < (int)io_list.size(); ++i )
        {
            prefs.set( Fl_Preferences::Name( "io-item-%d", i+1 ), io_list[i].c_str());
        }
    }
    else
    {
        atlas_host->activate();
        atlas_port->activate();
        int i, iVal, loc = 0;
        prefs.get("io-count", iVal, 0);
        for ( i = 1; i <= iVal; ++i )
        {
            char buf[256];
            prefs.get( Fl_Preferences::Name( "io-item-%d", i ), buf, "", sizeof buf - 1 );
            string item( buf );
            if ( item.length() > 8 && item.substr( 0, 8 ) == "--atlas=" )
            {
                loc = i;
                break;
            }
        }
        string host = atlas_host->value();
        if ( host.empty() )
            host = "localhost";
        int port = (int)atlas_port->value();
        if ( port == 0 )
            port = 5500;
        atlas_port->value(port);
        std::ostringstream opt;
        opt << "--atlas=socket,out,5," << host << "," << port << ",udp";
        if ( loc == 0 )
        {
            loc = iVal + 1;
            prefs.set("io-count",loc);
        }
        prefs.set( Fl_Preferences::Name( "io-item-%d", loc ), opt.str().c_str() );
    }
    update_options();
}

void
Wizard::atlas_host_cb()
{
    int i, iVal, loc = 0;
    prefs.get("io-count", iVal, 0);
    for ( i = 1; i <= iVal; ++i )
    {
        char buf[256];
        prefs.get( Fl_Preferences::Name( "io-item-%d", i ), buf, "", sizeof buf - 1 );
        string item( buf );
        if ( item.length() > 8 && item.substr( 0, 8 ) == "--atlas=" )
        {
            loc = i;
            break;
        }
    }
    string host = atlas_host->value();
    if ( host.empty() )
        host = "localhost";
    int port = (int)atlas_port->value();
    if ( port == 0 )
        port = 5500;
    atlas_port->value(port);
    std::ostringstream opt;
    opt << "--atlas=socket,out,5," << host << "," << port << ",udp";
    if ( loc == 0 )
    {
        loc = iVal + 1;
        prefs.set("io-count",loc);
    }
    prefs.set( Fl_Preferences::Name( "io-item-%d", loc ), opt.str().c_str() );
    update_options();
}

void
Wizard::atlas_port_cb()
{
    int i, iVal, loc = 0;
    prefs.get("io-count", iVal, 0);
    for ( i = 1; i <= iVal; ++i )
    {
        char buf[256];
        prefs.get( Fl_Preferences::Name( "io-item-%d", i ), buf, "", sizeof buf - 1 );
        string item( buf );
        if ( item.length() > 8 && item.substr( 0, 8 ) == "--atlas=" )
        {
            loc = i;
            break;
        }
    }
    string host = atlas_host->value();
    if ( host.empty() )
        host = "localhost";
    int port = (int)atlas_port->value();
    if ( port == 0 )
        port = 5500;
    std::ostringstream opt;
    opt << "--atlas=socket,out,5," << host << "," << port << ",udp";
    if ( loc == 0 )
    {
        loc = iVal + 1;
        prefs.set("io-count",loc);
    }
    prefs.set( Fl_Preferences::Name( "io-item-%d", loc ), opt.str().c_str() );
    update_options();
}

void
Wizard::multiplay_cb()
{
    int v = multiplay->value();
    if ( v == 0 )
    {
        multiplay_callsign->deactivate();
        multiplay_host->deactivate();
        multiplay_in->deactivate();
        multiplay_out->deactivate();
        prefs.set("callsign","");
        prefs.set("multiplay1","");
        prefs.set("multiplay2","");
    }
    else
    {
        multiplay_callsign->activate();
        multiplay_host->activate();
        multiplay_in->activate();
        multiplay_out->activate();

        string callsign = multiplay_callsign->value();
        string host = multiplay_host->value();
        int in = (int)multiplay_in->value();
        int out = (int)multiplay_out->value();

        prefs.set("callsign",callsign.c_str());
        std::ostringstream str;
        str << "out,10," << host << "," << out;
        prefs.set("multiplay1",str.str().c_str());
        str.str("");
        str << "in,10,," << in;
        prefs.set("multiplay2",str.str().c_str());

        ai_models->value(1);
        ai_models_cb();
    }
    update_options();
}

void
Wizard::multiplay_field_cb()
{
    string callsign = multiplay_callsign->value();
    string host = multiplay_host->value();
    int in = (int)multiplay_in->value();
    int out = (int)multiplay_out->value();

    prefs.set("callsign",callsign.c_str());
    std::ostringstream str;
    str << "out,10," << host << "," << out;
    prefs.set("multiplay1",str.str().c_str());
    str.str("");
    str << "in,10,," << in;
    prefs.set("multiplay2",str.str().c_str());
    update_options();
}

void
Wizard::multiplay_callsign_cb()
{
    string callsign = multiplay_callsign->value();
    string::size_type p;
    if ((p = callsign.find(' ')) != string::npos)
    {
        callsign.erase(p,1);
        multiplay_callsign->value(callsign.c_str());
    }
    else
        multiplay_field_cb();
}

void
Wizard::update_basic_options( Fl_Preferences &p )
{
    const int buflen = 256;
    char buf[ buflen ];

    p.get("geometry", buf, "", buflen-1);
    if ( buf[0] == '\0' )
    {
        strcpy( buf, "800x600" );
    }
    int i;
    for ( i = 0; menu_resolution[i].text != 0; i++ )
    {
        if ( strcmp( buf, menu_resolution[i].text ) == 0 )
        {
            resolution->value(i);
            break;
        }
    }
    if ( menu_resolution[i].text == 0 )
    {
        const char *t = resolution->text(i);
        if ( t == 0 )
            resolution->add( buf );
        else if ( strcmp( t, buf ) != 0 )
        {
            resolution->remove(i);
            resolution->add( buf );
        }
        resolution->value(i);
    }

    p.get("bpp", buf, "32", buflen-1);
    set_choice( bpp, buf );

    int iVal, iVal2;
    p.get("fullscreen", iVal, 0);
    game_mode->value(iVal);
    p.get("horizon_effect", iVal, 0);
    horizon_effect->value(iVal);
    p.get("enhanced_lighting", iVal, 0);
    enhanced_lighting->value(iVal);
    p.get("specular_highlight", iVal, 0);
    specular_highlight->value(iVal);
    p.get("clouds3d", iVal, 0);
    clouds_3d->value(iVal);
    p.get("frame_rate_limiter", iVal, 0);
    frame_rate_limiter->value(iVal);
    p.get("frame_rate_limiter_value", iVal2, 60);
    frame_rate_limiter_value->value( iVal2 );
    if ( iVal )
        frame_rate_limiter_value->activate();
    else
        frame_rate_limiter_value->deactivate();

    p.get("random_objects", iVal, 0);
    random_objects->value(iVal);
    p.get("random_trees", iVal, 0);
    random_trees->value(iVal);
    p.get("ai_models", iVal, 0);
    ai_models->value(iVal);
    p.get("ai_traffic", iVal, 0);
    ai_traffic->value(iVal);
    if (ai_models->value() == 0)
        ai_traffic->deactivate();
    else
        ai_traffic->activate();
    p.get("time_of_day", iVal, 0);
    time_of_day->value(iVal);
    p.get("time_of_day_value", buf, "noon", buflen-1);
    set_choice( time_of_day_value, buf );
    p.get("season", buf, "summer", buflen-1);
    set_choice( season, buf );
    p.get("fetch_real_weather", iVal, 0);
    real_weather_fetch->value(iVal);
    p.get("auto_coordination", iVal, 0);
    auto_coordination->value(iVal);
    p.get("terrasync", iVal, 0);
    terrasync->value(iVal);
    p.get("autovisibility", iVal, 0);
    auto_visibility->value(iVal);
	
    p.get("show_console", iVal, 0);
    show_console->value(iVal);

    atlas->value(0);
    atlas_host->value("");
    atlas_host->deactivate();
    atlas_port->value(0);
    atlas_port->deactivate();
    p.get("io-count", iVal, 0);
    for ( i = 1; i <= iVal; ++i )
    {
        buf[0] = 0;
        p.get( Fl_Preferences::Name("io-item-%d", i), buf, "", buflen-1 );
        string item( buf );
        if ( item.length() > 19 && item.substr( 0, 19 ) == "--atlas=socket,out," )
        {
            item.erase( 0, 19 );
            string::size_type p = item.find( ',' );
            if ( p != string::npos )
            {
                item.erase( 0, p+1 );
                p = item.find( ',' );
                if ( p != string::npos )
                {
                    string host = item.substr(0, p);
                    item.erase( 0, p+1 );
                    p = item.find( ',' );
                    if ( p != string::npos )
                    {
                        string port = item.substr(0, p);
                        item.erase(0, p+1);
                        if ( item == "udp" )
                        {
                            atlas->value(1);
                            atlas_host->value( host.c_str() );
                            atlas_port->value( atoi( port.c_str() ) );
                            atlas_host->activate();
                            atlas_port->activate();
                        }
                    }
                }
            }
        }
    }

    multiplay->value(0);
    multiplay_callsign->value("");
    multiplay_host->value("");
    multiplay_in->value(0);
    multiplay_out->value(0);
    multiplay_callsign->deactivate();
    multiplay_host->deactivate();
    multiplay_in->deactivate();
    multiplay_out->deactivate();

    p.get("callsign", buf, "", buflen-1 );
    string callsign(buf);
    p.get("multiplay1", buf, "", buflen-1 );
    string multiplay1(buf);
    p.get("multiplay2", buf, "", buflen-1 );
    string multiplay2(buf);

    if ( callsign.size() && multiplay1.size() && multiplay2.size() )
    {
        if ( multiplay1.size() >= 3 && multiplay2.size() >= 3 )
        {
            if ( multiplay1.substr(0,3) != "out" || multiplay2.substr(0,3) != "in," )
            {
                string tmp = multiplay1;
                multiplay1 = multiplay2;
                multiplay2 = tmp;
            }
            if ( multiplay1.substr(0,3) == "out" && multiplay2.substr(0,3) == "in," )
            {
                if ( multiplay1.size() >= 4 )
                {
                    multiplay1.erase(0, 4);
                    multiplay2.erase(0, 3);

                    string::size_type p = multiplay1.find(',');
                    if ( p != string::npos )
                    {
                        multiplay1.erase(0, p+1);
                        p = multiplay2.find(',');
                        if ( p != string::npos )
                        {
                            multiplay2.erase(0, p+1);
                            p = multiplay1.find(',');
                            if ( p != string::npos )
                            {
                                string host = multiplay1.substr(0, p);
                                multiplay1.erase(0, p+1);
                                p = multiplay2.find(',');
                                if ( p != string::npos )
                                {
                                    multiplay2.erase(0, p+1);
                                    int out = atoi(multiplay1.c_str());
                                    int in = atoi(multiplay2.c_str());

                                    multiplay->value(1);
                                    multiplay_callsign->value(callsign.c_str());
                                    multiplay_callsign->activate();
                                    multiplay_host->value(host.c_str());
                                    multiplay_host->activate();
                                    multiplay_in->value(in);
                                    multiplay_in->activate();
                                    multiplay_out->value(out);
                                    multiplay_out->activate();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void
Wizard::display_scenarii()
{
    set<string> selected;
    int iVal;
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];
    prefs.get( "scenario-count", iVal, 0 );
    int i;
    for (i = 1; i <= iVal; ++i)
    {
        buf[0] = 0;
        prefs.get( Fl_Preferences::Name("scenario-item-%d", i), buf, "", buflen-1 );
        if ( strlen( buf ) > 0 )
            selected.insert( buf );
    }

    scenarii->clear();

    SGPath path( fg_root_->value() );
    path.append( "AI" );

    i = 1;
    simgear::Dir directory( path );
    simgear::PathList files = directory.children( simgear::Dir::TYPE_FILE | simgear::Dir::NO_DOT_OR_DOTDOT );
    for ( simgear::PathList::iterator ii = files.begin(); ii != files.end(); ++ii )
    {
        if (fl_filename_match(ii->c_str(), "*.xml"))
        {
            string n = ii->file();
            n.erase(n.size()-4);
            scenarii->add( n.c_str() );
            if ( selected.find( n ) != selected.end() )
                scenarii->select(i);
            i += 1;
        }
    }

    scenarii_cb();
}

void
Wizard::deselect_all_scenarii_cb()
{
    scenarii->deselect();
    scenarii_help->tooltip( _("Select a scenario to display its description") );

    prefs.set("scenario-count", 0);
    update_options();
}

void
Wizard::show_cmd_line_cb()
{
    if ( show_cmd_line->value() )
        text->show();
    else
        text->hide();
    prefs.set("show_cmd_line", show_cmd_line->value());
}

void
Wizard::show_console_cb()
{
    prefs.set("show_console", show_console->value());
    update_options();
}

Wizard::FlightGearThread::FlightGearThread( Wizard *w )
: wizard( w ), viewer( false )
{
}

void
Wizard::FlightGearThread::setViewer(bool v)
{
    viewer = v;
}

void
Wizard::FlightGearThread::run()
{
    std::ostringstream ostr;
    if ((viewer && write_fgviewerrc( wizard->prefs, ostr, " ", wizard->current_aircraft_model_path )) || wizard->write_fgfsrc( wizard->prefs, ostr, " " ))
    {
        wizard->launch_result = wizard->run_fgfs(ostr.str());
    }
}

void
Wizard::exec_launch_window()
{
    int X = win->x(),
        Y = win->y(),
        W = win->w(),
        H = win->h(),
        w = launch_window->w(),
        h = launch_window->h();
    launch_window->position( X + ( W - w ) / 2, Y + ( H - h ) / 2 );
    launch_result = -1;
    launch_window->set_modal();
    launch_window->show();
    while ( launch_result == -1 )
    {
        Fl::wait();
    }
    launch_window->set_non_modal();
    launch_window->hide();

    if ( launch_result != 0 )
    {
        exec_crash_window( dump_file_name.c_str() );
    }
}

void
Wizard::reset_settings()
{
    prefs.deleteEntry( "fg_exe" );
    prefs.deleteEntry( "fg_root" );
    prefs.deleteEntry( "fg_scenery" );
    prefs.deleteEntry( "aircraft" );
 
    prefs.set( "time_of_day", 1 );
    prefs.set( "time_of_day_value", "noon" );

    aircraft->value( 0 );
    preview->make_current();
    preview->clear();
    preview->redraw();

    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

    if ( prefs.get( "fg_exe_init", buf, "", buflen-1) != 0 )
    {
        prefs.set( "fg_exe", buf );
    }

    if ( prefs.get( "fg_root_init", buf, "", buflen-1) != 0 )
    {
        prefs.set( "fg_root", buf );
    }

    if ( prefs.get( "fg_scenery_init", buf, "", buflen-1 ) != 0 )
    {
        prefs.set( "fg_scenery", buf );
    }

    reset();
}

void
Wizard::save_basic_options( Fl_Preferences &p )
{
    char abs_name[ FL_PATH_MAX ];
    fl_filename_absolute( abs_name, fg_exe_->value() );
    p.set( "fg_exe", abs_name );

    fl_filename_absolute( abs_name, fg_root_->value() );
    p.set( "fg_root", abs_name );

    string fg_scenery;
    for (int i = 1; i <= scenery_dir_list_->size(); ++i)
    {
        if (i != 1) fg_scenery += os::searchPathSep;
        std::string path = scenery_dir_list_->text(i);
        std::string::size_type p;
        if ((p = path.find('\t')) != std::string::npos)
            path.erase( p );
        fg_scenery += path;
    }

    p.set( "fg_scenery", fg_scenery.c_str() );
    int n = aircraft->value();
    if (n > 0)
    {
        AircraftData* data =
            reinterpret_cast<AircraftData*>( aircraft->data(n) );
        p.set( "aircraft", n > 0 ? data->name.c_str() : "" );
    }
    p.set( "airport", airports_->get_selected_id().c_str() );
    p.set( "airport-name",
                airports_->get_selected_name().c_str() );
    p.set( "carrier", carrier_->value() );
    if ( carrier_->value() != string() )
        p.set( "parkpos", parkpos_->value() );
    else
        p.set( "parkpos", airports_->get_selected_parking().c_str() );

    string rwy( airports_->get_selected_runway() );
    if (rwy.empty())
        rwy = _("<default>");
    p.set( "runway", rwy.c_str() );

    p.set("geometry", resolution->text());
    p.set("bpp", bpp->text());
    p.set("fullscreen", game_mode->value());
    p.set("horizon_effect", horizon_effect->value());
    p.set("enhanced_lighting", enhanced_lighting->value());
    p.set("specular_highlight", specular_highlight->value());
    p.set("clouds3d", clouds_3d->value());
    int v = frame_rate_limiter->value();
    if ( v == 0 )
    {
        frame_rate_limiter_value->deactivate();
    }
    else
    {
        frame_rate_limiter_value->activate();
    }
    p.set("frame_rate_limiter", v);
    p.set("frame_rate_limiter_value", frame_rate_limiter_value->value() );
    p.set("random_objects", random_objects->value());
    p.set("random_trees", random_trees->value());
    p.set("ai_models", ai_models->value());

    v = time_of_day->value();
    if ( v == 0 )
    {
        time_of_day_value->deactivate();
        p.set("time-match-real", 1);
    }
    else
    {
        time_of_day_value->activate();
        p.set("time-match-real", 0);
        p.set("time-match-local", 0);
        p.set("start-date-sys", 0);
        p.set("start-date-gmt", 0);
        p.set("start-date-lat", 0);
    }
    p.set("time_of_day", v);

    p.set("time_of_day_value", (const char *)time_of_day_value->mvalue()->user_data_);
    p.set("season", (const char *)season->mvalue()->user_data_);
    p.set("fetch_real_weather", real_weather_fetch->value());
    p.set("auto_coordination", auto_coordination->value());

    int nb = 0;
    for (int i = 1; i <= scenarii->size(); ++i)
    {
        if ( scenarii->selected(i) )
        {
            nb += 1;
            p.set( Fl_Preferences::Name("scenario-item-%d", nb), scenarii->text(i));
        }
    }
    p.set("scenario-count", nb);

    v = atlas->value();
    if ( v == 0 )
    {
        std::vector<string> io_list;
        int i, iVal;
        p.get("io-count", iVal, 0);
        for ( i = 1; i <= iVal; ++i )
        {
            char buf[256];
            p.get( Fl_Preferences::Name( "io-item-%d", i ), buf, "", sizeof buf - 1 );
            string item( buf );
            if ( item.length() < 8 || item.substr( 0, 8 ) != "--atlas=" )
            {
                io_list.push_back(buf);
            }
        }
        p.set("io-count",(int)io_list.size());
        for ( i = 0; i < (int)io_list.size(); ++i )
        {
            p.set( Fl_Preferences::Name( "io-item-%d", i+1 ), io_list[i].c_str());
        }
    }
    else
    {
        int i, iVal, loc = 0;
        p.get("io-count", iVal, 0);
        for ( i = 1; i <= iVal; ++i )
        {
            char buf[256];
            p.get( Fl_Preferences::Name( "io-item-%d", i ), buf, "", sizeof buf - 1 );
            string item( buf );
            if ( item.length() > 8 && item.substr( 0, 8 ) == "--atlas=" )
            {
                loc = i;
                break;
            }
        }
        string host = atlas_host->value();
        if ( host.empty() )
            host = "localhost";
        int port = (int)atlas_port->value();
        if ( port == 0 )
            port = 5500;
        atlas_port->value(port);
        std::ostringstream opt;
        opt << "--atlas=socket,out,5," << host << "," << port << ",udp";
        if ( loc == 0 )
        {
            loc = iVal + 1;
            p.set("io-count",loc);
        }
        p.set( Fl_Preferences::Name( "io-item-%d", loc ), opt.str().c_str() );
    }

    v = multiplay->value();
    if ( v == 0 )
    {
        p.set("callsign","");
        p.set("multiplay1","");
        p.set("multiplay2","");
    }
    else
    {
        string callsign = multiplay_callsign->value();
        string host = multiplay_host->value();
        int in = (int)multiplay_in->value();
        int out = (int)multiplay_out->value();

        p.set("callsign",callsign.c_str());
        std::ostringstream str;
        str << "out,10," << host << "," << out;
        p.set("multiplay1",str.str().c_str());
        str.str("");
        str << "in,10,," << in;
        p.set("multiplay2",str.str().c_str());
    }


}

void
Wizard::load_preferences_cb()
{
    char *filename = fl_file_chooser( _("Load settings from..."), "*.fgrun", "settings.fgrun" );
    if ( filename )
    {
        win->cursor( FL_CURSOR_WAIT );

        SGPath set1( filename ),
                set2( filename );
        set2.concat( ".prefs" );
        rename( filename, set2.c_str() );
        {
        Fl_Preferences prefs_tmp( set1.dir().c_str(), "flightgear.org", set1.file().c_str() );

        if (adv == 0)
            adv = new Advanced( prefs );

        adv->load_settings( prefs_tmp );
        update_basic_options( prefs_tmp );

        const int buflen = FL_PATH_MAX;
        char buf[ buflen ];
        prefs_tmp.get( "fg_exe", buf, def_fg_exe.c_str(), buflen-1);
        fg_exe_->value( buf );

        prefs_tmp.get( "fg_root", buf, def_fg_root.c_str(), buflen-1);
        fg_root_->value( buf );

        if (prefs_tmp.get( "fg_scenery", buf, "", buflen-1))
        {
            scenery_dir_list_->clear();
            typedef vector<string> vs_t;
            vs_t v( sgPathSplit( buf ) );

            for (vs_t::size_type i = 0; i < v.size(); ++i)
            {
                scenery_dir_list_->add( v[i].c_str() );
            }
        }

        prefs_tmp.get( "aircraft", buf, "", buflen-1);
        aircraft_update( buf );

        if (prefs_tmp.get( "airport", buf, "", buflen-1) && buf[0] != 0)
            airports_->select_id( buf );

        prefs_tmp.get( "carrier", buf, "", buflen-1);
        carrier_->value( buf );
        prefs_tmp.get( "parkpos", buf, "", sizeof buf - 1 );
        if ( carrier_->value() != string() )
            parkpos_->value( buf );
        else
            airports_->select_parking( buf );
        if (prefs_tmp.get( "runway", buf, "", buflen-1) && buf[0] != 0)
            airports_->select_rwy( buf );

        int iVal;
        prefs_tmp.get( "scenario-count", iVal, 0 );
        prefs.set( "scenario-count", iVal );
        int i;
        for (i = 1; i <= iVal; ++i)
        {
            buf[0] = 0;
            prefs_tmp.get( Fl_Preferences::Name("scenario-item-%d", i), buf, "", buflen-1 );
            prefs.set( Fl_Preferences::Name("scenario-item-%d", i), buf );
        }

        adv->save_settings( prefs );
        display_scenarii();
        save_basic_options( prefs );

        update_options();
        }
        rename( set2.c_str(), filename );

        win->cursor( FL_CURSOR_DEFAULT );
    }
}

void
Wizard::save_preferences_cb()
{
    char *filename = 0;
    for (;;)
    {
        filename = fl_file_chooser( _("Save settings as..."), "*.fgrun", "settings.fgrun" );
        struct stat stat_info;
        if ( !filename ||
                stat( filename, &stat_info ) != 0 ||
                fl_choice( _("Filename \"%s\" already exists."), _("Overwrite"), _("Don't overwrite"), 0, fl_filename_name( filename ) ) == 0 )
        {
            break;
        }
    }
    if ( filename )
    {
        SGPath settings( filename );
        {
        Fl_Preferences prefs_tmp( settings.dir().c_str(), "flightgear.org", settings.file().c_str() );

        if (adv == 0)
            adv = new Advanced( prefs );

        adv->save_settings( prefs_tmp );
        save_basic_options( prefs_tmp );
        }
        settings.concat( ".prefs" );
        unlink( filename );
        rename( settings.c_str(), filename );
    }
}

void
Wizard::crash_ok_cb()
{
    crash_window->set_non_modal();
    crash_window->hide();
}

void
Wizard::exec_crash_window( const char *fname )
{
    int X = win->x(),
        Y = win->y(),
        W = win->w(),
        H = win->h(),
        w = crash_window->w(),
        h = crash_window->h();
    file_box->copy_label( fname );
    crash_window->position( X + ( W - w ) / 2, Y + ( H - h ) / 2 );
    crash_window->set_modal();
    crash_window->show();
}

void
Wizard::start_viewer_cb()
{
    prefs.flush();
    if (fgThread == 0)
        fgThread = new FlightGearThread( this );
    fgThread->setViewer(true);
    fgThread->start();

    exec_launch_window();
}

void 
Wizard::auto_visibility_cb()
{
    prefs.set("autovisibility", auto_visibility->value());
    update_options();
}

void
Wizard::show_3d_preview_cb()
{
    prefs.set("show_3d_preview", show_3d_preview->value());
    preview_aircraft(false);
}

void
Wizard::aircraft_mru_update()
{
    aircraft_mru->clear();

    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];
    int nb = 0;
    prefs.get("aircraft-mru-count", nb, 0);
    for (int i = 1; i <= nb; ++i)
    {
        if (prefs.get( Fl_Preferences::Name("aircraft-mru-item-%d", i), buf, "", buflen-1))
        {
            string name(buf);
            size_t p = name.find(';');
            if (p != string::npos)
                name.erase(0, p+1);
            aircraft_mru->add(name.c_str());
        }
    }
}

void
Wizard::update_aircraft_mru()
{
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];
    char buf1[ buflen ];
    int pos = 0;
    if (prefs.get( "aircraft", buf, "", buflen-1 ) && buf[0] != 0 &&
        prefs.get( "aircraft_name", buf1, "", buflen-1 ) && buf1[0] != 0)
    {
        deque<string> aircraft_mru;

        int nb = 0;
        prefs.get("aircraft-mru-count", nb, 0);
        for (int i = 1; i <= nb; ++i)
        {
            char buf2[ buflen ];
            if (prefs.get( Fl_Preferences::Name("aircraft-mru-item-%d", i), buf2, "", buflen))
            {
                aircraft_mru.push_back(buf2);
                if (aircraft_mru.back().find(buf) == 0 && aircraft_mru.back().size() > strlen(buf) && aircraft_mru.back()[strlen(buf)] == ';')
                    pos = i;
            }
        }
        if (pos != 0)
        {
            aircraft_mru.erase(aircraft_mru.begin() + (pos - 1));
        }
        aircraft_mru.push_front(string(buf)+";"+buf1);
        while (aircraft_mru.size() > 5)
            aircraft_mru.pop_back();

        for (unsigned int i = 1; i <= aircraft_mru.size(); ++i)
        {
            prefs.set( Fl_Preferences::Name("aircraft-mru-item-%d", i), aircraft_mru[i-1].c_str());
        }
        prefs.set("aircraft-mru-count", (int)aircraft_mru.size());
    }
}

void
Wizard::aircraft_from_mru()
{
    int n = aircraft_mru->value();
    if (n == 0)
        return;

    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];
    if (prefs.get( Fl_Preferences::Name("aircraft-mru-item-%d", n), buf, "", buflen))
    {
        string name(buf);
        size_t p = name.find(';');
        if (p != string::npos)
            name.erase(0, p+1);

        string search = string("    ") + name;
        for (int i=1; i <= aircraft->size(); ++i)
        {
            string line = aircraft->text(i);
            if (line == search)
            {
                aircraft->value(i);
                preview_aircraft(false);
                return;
            }
        }
    }
}
