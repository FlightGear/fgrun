#include <FL/Fl.H>
#include <plib/ssg.h>

#include "Fl_Plib.h"

Fl_Plib::Fl_Plib(int x,int y,int w,int h,const char *l)
    : Fl_Gl_Window(x,y,w,h,l)
    , scene(0)
    , go(0)
{
  reset();
}

void
Fl_Plib::reset()
{
  EyeDist = 10.0f;
  EyeAz = 45.0f;
  EyeEl = 10.0f;
  Ex = 0.0f;
  Ey = 0.0f;
  Ez = 0.0f;
  EL_SENS = 0.3f;
  AZ_SENS = 0.3f;
}

void
Fl_Plib::init()
{
  if (go == 1)
    return;

  //make_current();
  ssgInit();
  ssgSetFOV( 60.0f,
             atan(tan(30.0 * SG_DEGREES_TO_RADIANS) * h() / w() ) *
	     SG_RADIANS_TO_DEGREES * 2 );
   ssgSetNearFar( 1.0f, 700.0f );

  sgVec3 sunposn;
  sgSetVec3( sunposn, 0.2f, -0.5f, 0.5f );
  ssgGetLight(0)->setPosition( sunposn );

  scene = new ssgRoot;
  go = 1;
}

using std::string;

ssgEntity*
Fl_Plib::load( const string& fname )
{
    string dir(fname);
    string::size_type pos = dir.rfind( "/" );
    if (pos != string::npos)
    {
	dir.erase( pos, string::npos );
	ssgTexturePath( (char*) dir.c_str() );
    }

    ssgEntity *obj = ssgLoad( (char*) fname.c_str() );
    return obj;
}

void
Fl_Plib::set_model( ssgEntity* obj, ssgEntity* bounding_obj )
{
    if ( bounding_obj == 0 )
        bounding_obj = obj;
    EyeDist = bounding_obj->getBSphere()->getRadius() + 5.f;
    const float *center = bounding_obj->getBSphere()->getCenter();
    sgVec3 dir;
    sgSetVec3( dir, -center[0], -center[1], -center[2] );

    sgMat4 transMat;
    sgMakeTransMat4 ( transMat, dir ) ;
    ssgTransform *trans = new ssgTransform();
    trans->setTransform( transMat );
    trans->addKid( obj );
    scene->addKid( trans );
}

void
Fl_Plib::make_matrix(sgMat4 mat)
{
  SGfloat angle = -EyeAz * SG_DEGREES_TO_RADIANS ;
  sgVec3 eye ;
  eye[0] = (SGfloat) cos (angle) * EyeDist ;
  eye[1] = (SGfloat) sin (angle) * EyeDist ;
  angle = EyeEl * SG_DEGREES_TO_RADIANS ;
  eye[2] = (SGfloat) sin (angle) * EyeDist ;
  sgVec3 center ;
  sgSetVec3(center, Ex, Ey, Ez);
  sgVec3 up;
  sgSetVec3(up, 0.0f, 0.0f, 0.1f);
  sgMakeLookAtMat4(mat, eye, center, up);
}

void
Fl_Plib::draw()
{
    if (!valid())
    {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glShadeModel(GL_SMOOTH);
	glViewport(0,0,w(),h());
	if (go == 1)
	    ssgSetFOV( 60.0f,
		       atan(tan(30.0 * SG_DEGREES_TO_RADIANS) * h() / w() ) *
		       SG_RADIANS_TO_DEGREES * 2 );
    }

    glClearColor (0.14f, 0.18f, 0.2f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    if (go == 1)
    {
	sgMat4 mat;
	make_matrix(mat);
	ssgSetCamera(mat);
	ssgCullAndDraw(scene);
    }

    glPopMatrix();
}

void Fl_Plib::clear() {
  if (scene)
    scene->removeAllKids();
  reset();
}

int Fl_Plib::ConstrainEl() {
  if (EyeEl <= -90) {
    EyeEl = -89.99f;
    return 1;
    } else 
  if (EyeEl >= 90) {
    EyeEl = 89.99f;
    return 1;
    }
  return 0;
}

int
Fl_Plib::handle(int e)
{
//  fltk::focus(this);
    if (e == FL_PUSH)
    {
	downx = Fl::event_x();
	downy = Fl::event_y();
	switch (Fl::event_button())
	{
	case FL_LEFT_MOUSE:
	    downEl = EyeEl;
	    downAz = EyeAz;
	    break;

	case FL_RIGHT_MOUSE:
	    downDist = EyeDist;
	    break;

	case FL_MIDDLE_MOUSE:
	    downEx = Ex;
	    downEy = Ey;
	    downEz = Ez;
	    break;
	}
	return 1;
    }
    else 
	if (e == FL_RELEASE)
	{
	    return 1;
	}
	else 
	    if (e == FL_DRAG)
	    {
		int deltax = Fl::event_x() - downx;
		int deltay = Fl::event_y() - downy;

		switch (Fl::event_button())
		{
		case FL_LEFT_MOUSE:
		    EyeEl  = downEl + EL_SENS * deltay;
		    ConstrainEl();
		    EyeAz  = downAz + AZ_SENS * deltax;
		    break;

		case FL_RIGHT_MOUSE: {
		    float mult = 0.01f;
		    if (fabs(downDist) > 1.0f)
			mult = float(0.001 * fabs(downDist));
		    EyeDist = downDist + mult*deltay;
		    //std::cout << "EyeDist=" << EyeDist << "\n";
		    break;
		}

		case FL_MIDDLE_MOUSE: {
		    float mult = 0.01f;
		    if (fabs(EyeDist) > 1.0f)
			mult = float(0.0005f * fabs(EyeDist));
		    sgMat4 mat;
		    make_matrix(mat);
		    sgVec3 off;
		    sgSetVec3(off, mult*deltax, 0, -mult*deltay);
		    sgXformVec3(off, mat);
		    Ex -= off[0];
		    Ey -= off[1];
		    Ez -= off[2];
		    break;
		}
		} // switch
		redraw();
	    }
	    else 
		if (e == FL_KEYDOWN)
		{
		    int deltax = 10;
		    int deltay = 10;

		    switch (Fl::event_key())
		    {
		    case 65361: // left
			downEl = EyeEl;
			downAz = EyeAz;
			EyeEl  = downEl + EL_SENS * deltay;
			ConstrainEl();
			EyeAz  = downAz + AZ_SENS * deltax;
			break;
		    case 65363: // right
			downEl = EyeEl;
			downAz = EyeAz;
			EyeEl  = downEl - EL_SENS * deltay;
			ConstrainEl();
			EyeAz  = downAz - AZ_SENS * deltax;
			break;
		    case 65456: // ins
			downEl = EyeEl;
			downAz = EyeAz;
			EyeEl  = downEl - EL_SENS * deltay;
			ConstrainEl();
			EyeAz  = downAz + AZ_SENS * deltax;
			break;
		    case 65454: // del
			downEl = EyeEl;
			downAz = EyeAz;
			EyeEl  = downEl + EL_SENS * deltay;
			ConstrainEl();
			EyeAz  = downAz - AZ_SENS * deltax;
			break;
		    case 65366: { // PgDn
			downDist = EyeDist;
			float mult = 0.01f;
			if (fabs(downDist) > 1.0f)
			    mult = float(0.001 * fabs(downDist));
			EyeDist = downDist + mult*deltay;
			break;
		    }
		    case 65365: { // PgUp
			downDist = EyeDist;
			float mult = 0.01f;
			if (fabs(downDist) > 1.0f)
			    mult = float(0.001 * fabs(downDist));
			EyeDist = downDist - mult*deltay;
			break;
		    }
		    case 65362: { // up
			downEx = Ex;
			downEy = Ey;
			downEz = Ez;
			float mult = 0.01f;
			if ( fabs(EyeDist) > 1.0f )
			    mult = float( 0.0005f * fabs(EyeDist) );
			sgMat4 mat ;
			make_matrix( mat ) ;
			sgVec3 off;
			sgSetVec3( off, mult*deltax, 0, -mult*deltay );
			sgXformVec3( off, mat );
			Ex -= off[0];
			Ey -= off[1];
			Ez -= off[2];
			break;
		    }
		    case 65364: { // down
			downEx = Ex;
			downEy = Ey;
			downEz = Ez;
			float mult = 0.01f;
			if ( fabs(EyeDist) > 1.0f )
			    mult = float( 0.0005f * fabs(EyeDist) );
			sgMat4 mat ;
			make_matrix( mat ) ;
			sgVec3 off;
			sgSetVec3( off, mult*deltax, 0, -mult*deltay );
			sgXformVec3( off, mat );
			Ex += off[0];
			Ey += off[1];
			Ez += off[2];
			break;
		    }
		    } // switch
		    redraw();
		    return 1;
		}
		else
		    return Fl_Gl_Window::handle( e );
    return 0;
}

void
Fl_Plib::update()
{
    EyeAz += 1.0;
    redraw();
}
