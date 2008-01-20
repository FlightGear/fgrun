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
#include <set>
#include <sstream>
#include <string.h>

#include <FL/Fl.H>
#include <FL/Fl_Preferences.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

#include <simgear/props/props_io.hxx>
#include <simgear/structure/exception.hxx>
#include <simgear/misc/sg_path.hxx>

#include <osg/MatrixTransform>
#include <osgDB/ReadFile>

#include <plib/ul.h>

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
using std::set;

extern string def_fg_exe;
extern string def_fg_root;
extern string def_fg_scenery;

Fl_Menu_Item Wizard::menu_time_of_day_value[] = {
 {N_("noon"), 0,  0, (void*)"noon", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {N_("dusk"), 0,  0, (void*)"dusk", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {N_("midnight"), 0,  0, (void*)"midnight", 0, FL_NORMAL_LABEL, 0, 14, 0},
 {N_("dawn"), 0,  0, (void*)"dawn", 0, FL_NORMAL_LABEL, 0, 14, 0},
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
 * Make an offset matrix from rotations and position offset.
 */
void
sgMakeOffsetsMatrix( sgMat4 * result, double h_rot, double p_rot, double r_rot,
                     double x_off, double y_off, double z_off )
{
  sgMat4 rot_matrix;
  sgMat4 pos_matrix;
  sgMakeRotMat4(rot_matrix, h_rot, p_rot, r_rot);
  sgMakeTransMat4(pos_matrix, x_off, y_off, z_off);
  sgMultMat4(*result, pos_matrix, rot_matrix);
}

/**
 * Data associated with each entry in the aircraft browser.
 */
struct AircraftData
{
    SGPropertyNode props;
    string name;
    string desc;
    string status;
    string author;
};

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
	{
	    airports_->select_id( buf );
	}
    }
}

static const char* about_text = N_("\
<html>\
 <head>\
  <title>FlightGear Launch Control %s</title>\
 </head>\
 <body>\
  <h1>FlightGear Launch Control %s</h1>\
  <p>This program is released under the GNU General Public License (http://www.gnu.org/copyleft/gpl.html).</p>\
  <p>Report bugs to http://sourceforge.net/projects/fgrun</p>\
 </body>\
</html>");

void
Wizard::reset()
{
    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];

    prefs.get( "fg_exe", buf, def_fg_exe.c_str(), buflen-1);
    fg_exe_->value( buf );

    prefs.get( "fg_root", buf, def_fg_root.c_str(), buflen-1);
    fg_root_->value( buf );
    if ( fg_root_->size() == 0 )
    {
        char *e = getenv( "FG_ROOT" );
        if ( e )
        {
            prefs.set( "fg_root", e );
            fg_root_->value( e );
        }
    }

    string fg_scenery;
    if (!def_fg_scenery.empty())
    {
	fg_scenery = def_fg_scenery;
    }
    else if (prefs.get( "fg_scenery", buf, "", buflen-1))
    {
	fg_scenery = buf;
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
    typedef vector<string> vs_t;
    vs_t v( sgPathSplit( fg_scenery ) );
    for (vs_t::size_type i = 0; i < v.size(); ++i)
	scenery_dir_list_->add( v[i].c_str() );

    if (fg_exe_->size() == 0 ||
	fg_root_->size() == 0 ||
        !is_valid_fg_root( fg_root_->value() ) ||
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
        aircraft_update();

        prev->activate();
	next->activate();
        page[1]->show();
    }
    next->label( _("Next") );

    int iVal;
    prefs.get("show_cmd_line", iVal, 0);
    show_cmd_line->value(iVal);
    if ( iVal )
	text->show();
    else
	text->hide();
}

void
Wizard::init( bool fullscreen )
{
    for ( int i = 0; menu_time_of_day_value[i].text != 0; ++i )
    {
        menu_time_of_day_value[i].text = _( menu_time_of_day_value[i].text );
    }
    time_of_day_value->menu( menu_time_of_day_value );

    static const int npages = 5;

    make_launch_window();

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
    ((Wizard*)v)->update_preview();
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
loadModel( const string &fg_root, const string &path,
               const SGPath& externalTexturePath )
{
    osg::ref_ptr<osg::Node> model;
    SGPropertyNode props;

    // Load the 3D aircraft object itself
    SGPath modelpath = path, texturepath = path;
    if ( !ulIsAbsolutePathName( path.c_str() ) ) {
        SGPath tmp = fg_root;
        tmp.append(modelpath.str());
        modelpath = texturepath = tmp;
    }

    // Check for an XML wrapper
    if (modelpath.str().substr(modelpath.str().size() - 4, 4) == ".xml") {
        readProperties(modelpath.str(), &props);
        if (props.hasValue("/path")) {
            modelpath = modelpath.dir();
            modelpath.append(props.getStringValue("/path"));
            if (props.hasValue("/texture-path")) {
                texturepath = texturepath.dir();
                texturepath.append(props.getStringValue("/texture-path"));
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
            kid = loadModel( fg_root, submodel, externalTexturePath );
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
Wizard::preview_aircraft()
{
    Fl::remove_timeout( timeout_handler, this );

    preview->make_current();

    preview->clear();
    preview->redraw();

    int n = aircraft->value();
    if (n == 0)
        return;

    AircraftData* data =
	reinterpret_cast<AircraftData*>( aircraft->data(n) );
    prefs.set( "aircraft", n > 0 ? data->name.c_str() : "" );

    if (data->props.hasValue( "/sim/model/path" ))
    {
	SGPath path( fg_root_->value() ), tpath;
	path.append( data->props.getStringValue( "/sim/model/path" ) );

	if (!path.exists())
	{
	    fl_alert( _("Model not found: '%s'"), path.c_str() );
	    return;
	}

        setlocale( LC_ALL, "C" );
	try
	{
            win->cursor( FL_CURSOR_WAIT );
	    Fl::flush();

            osg::ref_ptr<osg::Node> model = loadModel( fg_root_->value(), path.str(), SGPath() );
	    if (model != 0)
	    {
                osg::ref_ptr<osg::Node> bounding_obj = find_named_node( model.get(), "Aircraft" );
                preview->set_model( model.get(), bounding_obj.get() );

		Fl::add_timeout( update_period, timeout_handler, this );
	    }
            win->cursor( FL_CURSOR_DEFAULT );
            preview->redraw();
	}
	catch (const sg_exception& exc )
	{
	    fl_alert( exc.getFormattedMessage().c_str() );
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
            aircraft_update();
 	    win->cursor( FL_CURSOR_DEFAULT );
        }

	string fg_scenery = scenery_dir_list_->text(1);
	for (int i = 2; i <= scenery_dir_list_->size(); ++i)
	{
	    fg_scenery += os::searchPathSep;
	    fg_scenery += scenery_dir_list_->text(i);
	}

 	prefs.set( "fg_scenery", fg_scenery.c_str() );

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
	prefs.set( "parkpos", parkpos_->value() );

	string rwy( airports_->get_selected_runway() );
	if (rwy.empty())
	    rwy = _("<default>");
	prefs.set( "runway", rwy.c_str() );

	update_basic_options();
	display_scenarii();
    }
    else if (wiz->value() == page[3])
    {
	prefs.flush();

	int err = pthread_create( &th, 0, &Wizard::startFlightGear_cb, (void *)this );

	exec_launch_window();
	return;
    }

    wiz->next();

    if (wiz->value() == page[1])
    {
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
	parkpos_->value( buf );

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
	write_fgfsrc( ostr, "\n  " );
	text->buffer()->text( ostr.str().c_str() );
	next->label( _("Run") );
    }
}

void
Wizard::prev_cb()
{
    next->activate();
    next->label( _("Next") );
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
	adv = new Advanced;
    }
    adv->reset_settings( prefs );
    reset_settings();
}

void
Wizard::fg_exe_select_cb()
{
    char* p = fl_file_chooser( _("Select FlightGear executable"),
                              fg_exe_->value(), 0);
    if (p != 0)
        fg_exe_->value( p );

    fg_exe_update_cb();
}

void
Wizard::fg_exe_update_cb()
{
//     if (fg_exe_->size() == 0)
// 	return;
}

void
Wizard::fg_root_update_cb()
{
    next->deactivate();

    if (fg_root_->size() == 0)
        return;

    string dir( fg_root_->value() );

    // Remove trailing separator.
    if (os::isdirsep( dir[ dir.length() - 1 ] ))
    {
        dir.erase( dir.length() - 1 );
    }

    if (!is_valid_fg_root( dir ))
    {
        dir.append( "/data" );
        if (!is_valid_fg_root( dir ))
            return;
    }

    fg_root_->value( dir.c_str() );

    if (scenery_dir_list_->size() == 0)
    {
	// Derive FG_SCENERY from FG_ROOT. 
	string d( dir );
	d.append( "/Scenery" );
	if (!fl_filename_isdir( d.c_str() ))
	    return;

	scenery_dir_list_->add( d.c_str() );
    }

    next->activate();
}

void
Wizard::fg_root_select_cb()
{
    char* p = fl_dir_chooser( _("Select FG_ROOT directory"),
                              fg_root_->value(), 0);
    if (p != 0)
        fg_root_->value( p );

    fg_root_update_cb();
}

void
Wizard::advanced_cb()
{
    if (adv == 0)
    {
	adv = new Advanced;
    }

    prefs.set( "airport", airports_->get_selected_id().c_str() );
    prefs.set( "airport-name", airports_->get_selected_name().c_str() );

    int r = adv->exec( prefs );
    if (r)
    {
    }

    update_basic_options();

    // Update command text.
    std::ostringstream ostr;
    ostr << fg_exe_->value() << "\n  ";
    write_fgfsrc( ostr, "\n  " );
    text->buffer()->text( ostr.str().c_str() );
}

void
Wizard::update_preview()
{
    preview->update();
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

    ulDir *dh = ulOpenDir( s.c_str() );
    ulDirEnt *ent;
    while (dh != 0 && (ent = ulReadDir( dh )))
    {
        if (fl_filename_match(ent->d_name, "*-set.xml"))
        {
            SGPath d( dir );
            d.append( ent->d_name );
            ac.push_back( d );
        }
        else if (recursive &&
                 strcmp( ent->d_name, "CVS" ) != 0 &&
                 strcmp( ent->d_name, ".." ) != 0 &&
                 strcmp( ent->d_name, "." ) != 0 )
        {
            SGPath d( dir );
            d.append( ent->d_name );
            if (fl_filename_isdir( d.c_str() ))
            {
                search_aircraft_dir( d, false, ac );
            }
        }
    }
    ulCloseDir( dh );
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
    SGPath path( fg_root_->value() );
    path.append( "Aircraft" );
    vector< SGPath > ac;
    search_aircraft_dir( path, true, ac );

    // Empty the aircraft browser list.
    for (int i = 1; i <= aircraft->size(); ++i)
    {
        if ( aircraft->text(i)[0] != ' ' )
        {
	    AircraftData* data =
	        reinterpret_cast<AircraftData*>( aircraft->data(i) );
	    delete data;
        }
    }
    aircraft->clear();

    const int buflen = FL_PATH_MAX;
    char buf[ buflen ];
    prefs.get( "aircraft", buf, "", buflen-1);

    map<string,vector<AircraftData*>,ICompare> am;
    bool selected = false;
    // Populate the aircraft browser list.
    for (vector<SGPath>::size_type vi = 0; vi < ac.size(); ++vi)
    {
	string s( ac[vi].str() ), name( s );
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
		copyProperties( &props, &data->props );
		//data->props = props;
		data->name = ss;
                data->desc = desc;
                data->status = props.getStringValue( "/sim/status" );
                if ( data->status.empty() ) data->status = _( "Unknown" );
                data->author = props.getStringValue( "/sim/author" );
                if ( data->author.empty() ) data->author = _( "Unknown" );
                am[name].push_back( data );
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

	    if (buf[0] != 0 && strcmp( buf, data->name.c_str() ) == 0)
	    {
	        aircraft->select( aircraft->size() );
                selected = true;
	    }
        }
    }

    if ( selected )
        Fl::add_timeout( 0.1, delayed_preview, this );
}

Wizard::~Wizard()
{
    delete logwin;
    delete win;
    delete adv;
}

void
Wizard::cancel_cb()
{
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
Wizard::scenery_dir_add_cb()
{
    char* p = fl_dir_chooser( _("Select FG_SCENERY directory"), 0, 0);
    if (p != 0)
    {
	scenery_dir_list_->add( p );
	scenery_dir_list_->value( scenery_dir_list_->size() );
	scenery_dir_delete_->activate();
	scenery_dir_list_->select( scenery_dir_list_->size() );
    }
}

void
Wizard::scenery_dir_delete_cb()
{
    int n = scenery_dir_list_->value();
    if (n > 0)
	scenery_dir_list_->remove( n );

    if (scenery_dir_list_->size() == 0)
	scenery_dir_delete_->deactivate();
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

    vector<string> v;
    for (int i = 1; i <= scenery_dir_list_->size(); ++i)
    {
	SGPath dir( scenery_dir_list_->text(i) );
	dir.append( "Terrain" );
	if (fl_filename_isdir( dir.c_str() ))
	    v.push_back( dir.str() );
	else
	    v.push_back( scenery_dir_list_->text(i) );
    }

    SGPath path( fg_root_->value() );
    path.append( "/Airports/apt.dat.gz" );
    airports_->load_runways( path.str(), airports_cb, this );

    string cache( cache_file_->value() );
    airports_->load_airports( v, cache, airports_cb, this );
}

void
Wizard::update_options()
{
    // Update command text.
    std::ostringstream ostr;
    ostr << fg_exe_->value() << "\n  ";
    write_fgfsrc( ostr, "\n  " );
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
    prefs.set("game_mode", game_mode->value());
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
Wizard::random_objects_cb()
{
    prefs.set("random_objects", random_objects->value());
    update_options();
}

void
Wizard::ai_models_cb()
{
    prefs.set("ai_models", ai_models->value());
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
	    tooltip += scenario.getStringValue( "scenario/description" );

	    size_t p = 0;
	    while ( ( p = tooltip.find( '@', p ) ) != string::npos )
	    {
		tooltip.insert( p, "@" );
		p += 2;
	    }
	}
	catch ( const sg_exception& )
	{
	    tooltip = "";
	}
    }
    else
	tooltip = "";

    scenarii->tooltip( tooltip.c_str() );

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
	char hostname[256];
	gethostname( hostname, 256 );
	str << "in,10," << hostname << "," << in;
	prefs.set("multiplay2",str.str().c_str());
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
    char hostname[256];
    gethostname( hostname, 256 );
    str << "in,10," << hostname << "," << in;
    prefs.set("multiplay2",str.str().c_str());
    update_options();
}

void
Wizard::update_basic_options()
{
    const int buflen = 256;
    char buf[ buflen ];

    prefs.get("geometry", buf, "", buflen-1);
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

    prefs.get("bpp", buf, "32", buflen-1);
    set_choice( bpp, buf );

    int iVal;
    prefs.get("game_mode", iVal, 0);
    game_mode->value(iVal);
    prefs.get("horizon_effect", iVal, 0);
    horizon_effect->value(iVal);
    prefs.get("enhanced_lighting", iVal, 0);
    enhanced_lighting->value(iVal);
    prefs.get("specular_highlight", iVal, 0);
    specular_highlight->value(iVal);
    prefs.get("clouds3d", iVal, 0);
    clouds_3d->value(iVal);
    prefs.get("random_objects", iVal, 0);
    random_objects->value(iVal);
    prefs.get("ai_models", iVal, 0);
    ai_models->value(iVal);
    prefs.get("time_of_day", iVal, 0);
    time_of_day->value(iVal);
    prefs.get("time_of_day_value", buf, "noon", buflen-1);
    set_choice( time_of_day_value, buf );
    prefs.get("fetch_real_weather", iVal, 0);
    real_weather_fetch->value(iVal);
    prefs.get("auto_coordination", iVal, 0);
    auto_coordination->value(iVal);

    atlas->value(0);
    atlas_host->value("");
    atlas_host->deactivate();
    atlas_port->value(0);
    atlas_port->deactivate();
    prefs.get("io-count", iVal, 0);
    for ( i = 1; i <= iVal; ++i )
    {
	buf[0] = 0;
	prefs.get( Fl_Preferences::Name("io-item-%d", i), buf, "", buflen-1 );
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

    prefs.get("callsign", buf, "", buflen-1 );
    string callsign(buf);
    prefs.get("multiplay1", buf, "", buflen-1 );
    string multiplay1(buf);
    prefs.get("multiplay2", buf, "", buflen-1 );
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
    ulDir *dh = ulOpenDir( path.c_str() );
    ulDirEnt *ent;
    while (dh != 0 && (ent = ulReadDir( dh )))
    {
        if ( strcmp( ent->d_name, "CVS" ) != 0 &&
	     strcmp( ent->d_name, ".." ) != 0 &&
	     strcmp( ent->d_name, "." ) != 0 )
        {
            SGPath d( path );
            d.append( ent->d_name );
            if (!fl_filename_isdir( d.c_str() ) &&
		 fl_filename_match(ent->d_name, "*.xml"))
	    {
		string n( ent->d_name );
		n.erase(n.size()-4);
		scenarii->add( n.c_str() );
		if ( selected.find( n ) != selected.end() )
		    scenarii->select(i);
		i += 1;
            }
        }
    }
    ulCloseDir( dh );
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

void *
Wizard::startFlightGear_cb( void *d )
{
    static_cast<Wizard *>( d )->startFlightGear_cb();
    return 0;
}

void
Wizard::startFlightGear_cb()
{
    std::ostringstream ostr;
    if (write_fgfsrc( ostr, " " ))
    {
	run_fgfs(ostr.str());
	launch_result = 0;
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
    const char* not_set = "NOT SET";

    prefs.get( "fg_exe_init", buf, not_set, buflen-1);
    if ( strcmp( buf, not_set ) != 0 )
    {
        prefs.set( "fg_exe", buf );
    }

    prefs.get( "fg_root_init", buf, not_set, buflen-1);
    if ( strcmp( buf, not_set ) != 0 )
    {
        prefs.set( "fg_root", buf );
    }

    prefs.get( "fg_scenery_init", buf, not_set, buflen-1 );
    if ( strcmp( buf, not_set ) != 0 )
    {
        prefs.set( "fg_scenery", buf );
    }

    reset();
}
