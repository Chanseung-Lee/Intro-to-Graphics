#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Chan Seung Lee

// title of these windows:

const char *WINDOWTITLE = "OpenGL / GLUT Sample -- Joe Graphics";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE     = GL_LINEAR;
const GLfloat FOGDENSITY  = 0.30f;
const GLfloat FOGSTART    = 1.5f;
const GLfloat FOGEND      = 4.f;

// for lighting:

const float	WHITE[ ] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 10000;		// 10000 milliseconds = 10 seconds


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER

#define XSIDE	2.0			// length of the x side of the grid
#define X0      (-XSIDE/2.)		// where one side starts
#define NX	200			// how many points in x
#define DX	( XSIDE/(float)NX )	// change in x between the points

#define ZSIDE	2.0			// length of the z side of the grid
#define Z0      (-ZSIDE/2.)		// where one side starts
#define NZ	200			// how many points in z
#define DZ	( ZSIDE/(float)NZ )	// change in z between the points


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
GLuint	BoxList;				// object display list
GLuint	CyList;					// object display list
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;		// ORTHO or PERSP
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void			Axes( float );
void			HsvRgb( float[3], float [3] );
void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);
float			Unit(float [3]);

int		catDL;
int		duckDL;
int		bunnyDL;
int		GridDL;
int		colorNum;
int		lightType;

int		SphereDL, MarsDL, VenusDL, EarthDL, JupiterDL, SaturnDL, UranusDL, NeptuneDL, MercuryDL, SunDL;	// display lists
GLuint		MarsTex, VenusTex, EarthTex, JupiterTex, SaturnTex, UranusTex, NeptuneTex, MercuryTex, SunTex;		// texture object
int		me_SphereDL, v_SphereDL, e_SphereDL, ma_SphereDL, j_SphereDL, s_SphereDL, u_SphereDL, n_SphereDL, su_SphereDL;
int		Mer_C, Ven_C, Ear_C, Jup_C, Sat_C, Ura_C, Nep_C, Mar_C;

int		textureMode = 1;
int		lightingMode = 1;
int		texMode = 1;



// utility to create an array from 3 separate values:

float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float *
MulArray3( float factor, float array0[ ] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float *
MulArray3(float factor, float a, float b, float c )
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

#include "setmaterial.cpp"
#include "setlight.cpp"
#include "osusphere.cpp"
//#include "osucone.cpp"
//#include "osutorus.cpp"
#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
#include "glslprogram.cpp"
#include "CarouselHorse0.10.550"


// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display lists that **will not change**:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}




// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow( MainWindow );

	// erase the background:
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
#ifdef DEMO_DEPTH_BUFFER
	if( DepthBufferOn == 0 )
		glDisable( GL_DEPTH_TEST );
#endif


	// specify shading to be flat:

	glShadeModel( GL_FLAT );

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( NowProjection == ORTHO )
		glOrtho( -2.f, 2.f,     -2.f, 2.f,     0.1f, 1000.f );
	else
		gluPerspective( 70.f, 1.f,	0.1f, 1000.f );

	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// set the eye position, look-at position, and up-vector:

	gluLookAt( 2.f, 2.f, 4.f,     0.f, 0.f, 0.f,     0.f, 1.f, 0.f );

	// rotate the scene:

	glRotatef( (GLfloat)Yrot, 0.f, 1.f, 0.f );
	glRotatef( (GLfloat)Xrot, 1.f, 0.f, 0.f );

	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );

	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[NowColor][0] );
		glCallList( AxesList );
	}

	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable( GL_NORMALIZE );


	// draw the box object by calling up its display list:

	//glCallList( BoxList );

	float radius = 2.0;
	float theta = 0.f;
	float deltaTheta = 0.01f;

	float x = radius * cos(45 * Time);
	float z = radius * sin(45 * Time);

	float r = 1.;
	float g = 1.;
	float b = 0.3;

	//glCallList( GridDL );

	// Texture Stuff -------------------------------------------------------------------------------------
	if ( textureMode == 1 )
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);


	GLfloat ambientColor[] = { 0, 0, 0, 1.0f };
	GLfloat diffuseColor[] = { r, g, b, 1.0f };
	GLfloat specularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };  

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glEnable(GL_LIGHT0);
	SetPointLight(GL_LIGHT0, 0, 0, 0, r, g, b);
	lightingMode = 2;
	if (lightingMode == 1)
	{
		glDisable(GL_LIGHTING);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	else
	{
		glEnable(GL_LIGHTING);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	float theta2 = 90 + Time * (360.f);

	// Calculate the angle based on time and speed
	float angle = -6 * Time;

	// Calculate the position of the horse on the circle
	x = 2. * cos(angle);
	z = 2. * sin(angle);

	// Calculate the horse's orientation angle (to make it face forward)
	//float horseAngle = 360 * Time;

	float slow_time = Time * 5;
	float radius_scale = -0.5;
	float slow_time_2 = Time * 0.01;

	// Call the display list with the updated translation
	glPushMatrix(); // Push the current matrix
	glRotatef(360.f * slow_time, 0, 1, 0); // Rotation around the sun
	glTranslatef(0., 0., (0.35f * radius_scale) - 1);
	glRotatef(360.f * slow_time_2, 0, 1, 0);// Rotation around its own axis
	glCallList(MercuryDL); // Render the horse
	glPopMatrix(); // Restore the previous matrix

	// Call the display list with the updated translation
	glPushMatrix(); // Push the current matrix
	glRotatef(360.f * slow_time * 0.39, 0, 1, 0);
	glTranslatef(0., 0., 0.67f * radius_scale -1);
	glRotatef(360.f * slow_time_2 * 1.507, 0, 1, 0);// Rotation around its own axis
	glCallList(VenusDL); // Render the horse
	glPopMatrix(); // Restore the previous matrix

	// Call the display list with the updated translation
	glPushMatrix(); // Push the current matrix
	glRotatef(360.f * slow_time * 0.24, 0, 1, 0);
	glTranslatef(0., 0., 0.92f * radius_scale -1);
	glRotatef(360.f * slow_time_2 * 176, 0, 1, 0);// Rotation around its own axis
	glCallList(EarthDL); // Render the horse
	glPopMatrix(); // Restore the previous matrix

	// Call the display list with the updated translation
	glPushMatrix(); // Push the current matrix
	glRotatef(360.f * slow_time * 0.12, 0, 1, 0);
	glTranslatef(0., 0., 1.41f * radius_scale -1);
	glRotatef(360.f * slow_time_2 * 176, 0, 1, 0);// Rotation around its own axis
	glCallList(MarsDL); // Render the horse
	glPopMatrix(); // Restore the previous matrix

	// Call the display list with the updated translation
	glPushMatrix(); // Push the current matrix
	glRotatef(360.f * slow_time * 0.02, 0, 1, 0);
	glTranslatef(0., 0., 4.83f * radius_scale -1);
	glRotatef(360.f * slow_time_2 * 426.5, 0, 1, 0);// Rotation around its own axis
	glCallList(JupiterDL); // Render the horse
	glPopMatrix(); // Restore the previous matrix

	// Call the display list with the updated translation
	glPushMatrix(); // Push the current matrix
	glRotatef(360.f * slow_time * 0.0082, 0, 1, 0);
	glTranslatef(0., 0., 8.90f * radius_scale -1);
	glRotatef(360.f * slow_time_2 * 394.6, 0, 1, 0);// Rotation around its own axis
	glCallList(SaturnDL); // Render the horse
	glPopMatrix(); // Restore the previous matrix

	// Call the display list with the updated translation
	glPushMatrix(); // Push the current matrix
	glRotatef(360.f * slow_time * 0.003, 0, 1, 0);
	glTranslatef(0., 0., 17.87f * radius_scale -1);
	glRotatef(360.f * slow_time_2 * 238.6, 0, 1, 0);// Rotation around its own axis
	glCallList(UranusDL); // Render the horse
	glPopMatrix(); // Restore the previous matrix

	// Call the display list with the updated translation
	glPushMatrix(); // Push the current matrix
	glRotatef(360.f * slow_time * 0.0015, 0, 1, 0);
	glTranslatef(0., 0., 27.98f * radius_scale -1);
	glRotatef(360.f * slow_time_2 * 262.3, 0, 1, 0);// Rotation around its own axis
	glCallList(NeptuneDL); // Render the horse
	glPopMatrix(); // Restore the previous matrix

	glDisable(GL_LIGHTING);
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);  // Set the color to white
	glCallList(Mer_C);
	glCallList(Ven_C);
	glCallList(Ear_C);
	glCallList(Mar_C);
	glCallList(Jup_C);
	glCallList(Ura_C);
	glCallList(Nep_C);
	glCallList(Sat_C);
	glPopMatrix();
	glEnable(GL_LIGHTING);

	glDisable(GL_LIGHTING);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Call the display list with the updated translation
	glPushMatrix(); // Push the current matrix
	//glRotatef(360.f * slow_time * 0.0015, 0, 1, 0);
	glCallList(SunDL); // Render the horse
	glPopMatrix(); // Restore the previous matrix

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glPushMatrix();
	//SetPointLight(GL_LIGHT1, 0.f, 10.f, 0.f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHT0);
	glPopMatrix();
	glEnable(GL_LIGHTING);

	/*
	glPushMatrix();
	glTranslatef(x, 0.6f, z);
	glColor3f(r, g, b);
	glDisable(GL_LIGHTING);
	glutSolidSphere(0.1f, 5.f, 5.f);
	glPopMatrix();

	SetPointLight(GL_LIGHT0, x, 5.9f, z, r, g, b);

	glPushMatrix();
	//SetPointLight(GL_LIGHT1, 0.f, 10.f, 0.f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHT0);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	//theta += deltaTheta;
	*/

#ifdef DEMO_Z_FIGHTING
	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.f,   0.f, 1.f, 0.f );
			glCallList( BoxList );
		glPopMatrix( );
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	glDisable( GL_DEPTH_TEST );
	glColor3f( 0.f, 1.f, 1.f );
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0.f, 100.f,     0.f, 100.f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1.f, 1.f, 1.f );
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	NowColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	NowProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(float) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
#endif

	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc( Animate );

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:

	// Texture Init

	//int v_width, v_height;
	//unsigned char* venus_texture = BmpToTexture("venus.bmp", &v_width, &v_height);
	//int level = 0, ncomps = 3, border = 0;
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexImage2D(GL_TEXTURE_2D, level, ncomps, v_width, v_height, border, GL_RGB, GL_UNSIGNED_BYTE, venus_texture);

	// Mars Texture -----------------------------------------------------------------------------------------------

	int m_width, m_height;
	char* file = (char*)"mars.bmp";
	unsigned char* texture = BmpToTexture(file, &m_width, &m_height);
	if (texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", file);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", file, m_width, m_height);

	glGenTextures(1, &MarsTex);
	glBindTexture(GL_TEXTURE_2D, MarsTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

	// Venus Texture -----------------------------------------------------------------------------------------------

	int v_width, v_height;
	char* file_v = (char*)"venus.bmp";
	unsigned char* texture_v = BmpToTexture(file_v, &v_width, &v_height);
	if (texture_v == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", file_v);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", file_v, v_width, v_height);

	glGenTextures(1, &VenusTex);
	glBindTexture(GL_TEXTURE_2D, VenusTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, v_width, v_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_v);

	// Earth Texture -----------------------------------------------------------------------------------------------

	int e_width, e_height;
	char* e_file = (char*)"earth.bmp";
	unsigned char* e_texture = BmpToTexture(e_file, &e_width, &e_height);
	if (e_texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", e_file);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", e_file, e_width, e_height);

	glGenTextures(1, &EarthTex);
	glBindTexture(GL_TEXTURE_2D, EarthTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, e_width, e_height, 0, GL_RGB, GL_UNSIGNED_BYTE, e_texture);

	// Jupiter Texture -----------------------------------------------------------------------------------------------

	int j_width, j_height;
	char* j_file = (char*)"jupiter.bmp";
	unsigned char* j_texture = BmpToTexture(j_file, &j_width, &j_height);
	if (j_texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", j_file);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", j_file, j_width, j_height);

	glGenTextures(1, &JupiterTex);
	glBindTexture(GL_TEXTURE_2D, JupiterTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, j_width, j_height, 0, GL_RGB, GL_UNSIGNED_BYTE, j_texture);

	// Saturn Texture -----------------------------------------------------------------------------------------------

	int s_width, s_height;
	char* s_file = (char*)"saturn.bmp";
	unsigned char* s_texture = BmpToTexture(s_file, &s_width, &s_height);
	if (s_texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", s_file);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", s_file, s_width, s_height);

	glGenTextures(1, &SaturnTex);
	glBindTexture(GL_TEXTURE_2D, SaturnTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, s_width, s_height, 0, GL_RGB, GL_UNSIGNED_BYTE, s_texture);

	// Uranus Texture -----------------------------------------------------------------------------------------------

	int u_width, u_height;
	char* u_file = (char*)"uranus.bmp";
	unsigned char* u_texture = BmpToTexture(u_file, &u_width, &u_height);
	if (j_texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", j_file);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", u_file, u_width, u_height);

	glGenTextures(1, &UranusTex);
	glBindTexture(GL_TEXTURE_2D, UranusTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, u_width, u_height, 0, GL_RGB, GL_UNSIGNED_BYTE, u_texture);

	// Neptune Texture -----------------------------------------------------------------------------------------------

	int n_width, n_height;
	char* n_file = (char*)"neptune.bmp";
	unsigned char* n_texture = BmpToTexture(n_file, &n_width, &n_height);
	if (n_texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", n_file);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", n_file, n_width, n_height);

	glGenTextures(1, &NeptuneTex);
	glBindTexture(GL_TEXTURE_2D, NeptuneTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, n_width, n_height, 0, GL_RGB, GL_UNSIGNED_BYTE, n_texture);

	// Mercury Texture -----------------------------------------------------------------------------------------------

	int me_width, me_height;
	char* me_file = (char*)"mercury.bmp";
	unsigned char* me_texture = BmpToTexture(me_file, &me_width, &me_height);
	if (me_texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", me_file);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", me_file, me_width, me_height);

	glGenTextures(1, &MercuryTex);
	glBindTexture(GL_TEXTURE_2D, MercuryTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, me_width, me_height, 0, GL_RGB, GL_UNSIGNED_BYTE, me_texture);

	// Sun Texture -----------------------------------------------------------------------------------------------

	int su_width, su_height;
	char* su_file = (char*)"sun.bmp";
	unsigned char* su_texture = BmpToTexture(su_file, &su_width, &su_height);
	if (su_texture == NULL)
		fprintf(stderr, "Cannot open texture '%s'\n", su_file);
	else
		fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", su_file, su_width, su_height);

	glGenTextures(1, &SunTex);
	glBindTexture(GL_TEXTURE_2D, SunTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, su_width, su_height, 0, GL_RGB, GL_UNSIGNED_BYTE, su_texture);
}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{

	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow( MainWindow );

	// create the object:


	float radius = 1.0f; // Radius of the cylinder
	float height = 2.0f; // Height of the cylinder
	int numSegments = 50; // Number of segments to create the cylinder

	glutSetWindow(MainWindow);
	
	// Create the grid:

	GridDL = glGenLists(1);
	glNewList(GridDL, GL_COMPILE);
	SetMaterial(0.6f, 0.6f, 0.6f, 30.f);
	glNormal3f(0., 1., 0.);
	for (int i = 0; i < NZ; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < NX; j++)
		{
			// YGRID = 0
			glVertex3f(X0 + DX * (float)j, 0, Z0 + DZ * (float)(i + 0));
			glVertex3f(X0 + DX * (float)j, 0, Z0 + DZ * (float)(i + 1));
		}
		glEnd();
	}
	glEndList();

	float base_radius = 0.08;
	float planet_scale = 4;

	SphereDL = glGenLists(1);
	glNewList(SphereDL, GL_COMPILE);
	OsuSphere(base_radius, 100 , 100 );
	glEndList();

	me_SphereDL = glGenLists(1);
	glNewList(me_SphereDL, GL_COMPILE);
	OsuSphere(base_radius, 100, 100);
	glEndList();

	v_SphereDL = glGenLists(1);
	glNewList(v_SphereDL, GL_COMPILE);
	OsuSphere(base_radius*2.48, 100, 100);
	glEndList();
	
	e_SphereDL = glGenLists(1);
	glNewList(e_SphereDL, GL_COMPILE);
	OsuSphere(base_radius*2.61, 100, 100);
	glEndList();

	ma_SphereDL = glGenLists(1);
	glNewList(ma_SphereDL, GL_COMPILE);
	OsuSphere(base_radius*1.39, 100, 100);
	glEndList();

	j_SphereDL = glGenLists(1);
	glNewList(j_SphereDL, GL_COMPILE);
	OsuSphere(base_radius*28.66, 100, 100);
	glEndList();

	s_SphereDL = glGenLists(1);
	glNewList(s_SphereDL, GL_COMPILE);
	OsuSphere(base_radius*23.87, 100, 100);
	glEndList();

	u_SphereDL = glGenLists(1);
	glNewList(u_SphereDL, GL_COMPILE);
	OsuSphere(base_radius*10.4, 100, 100);
	glEndList();

	n_SphereDL = glGenLists(1);
	glNewList(n_SphereDL, GL_COMPILE);
	OsuSphere(base_radius*10.09, 100, 100);
	glEndList();

	su_SphereDL = glGenLists(1);
	glNewList(su_SphereDL, GL_COMPILE);
	OsuSphere(1, 100, 100);
	glEndList();

	MarsDL = glGenLists(1);
	glNewList(MarsDL, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, MarsTex);	// MarsTex must have already been created when this is called
	glPushMatrix();
		glScalef(0.53f, 0.53f, 0.53f);	// scale of mars sphere, from the table
		glCallList(ma_SphereDL);		// a dl can call another dl that has been previously created
		glPopMatrix();
	glEndList();

	VenusDL = glGenLists(1);
	glNewList(VenusDL, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, VenusTex);	// MarsTex must have already been created when this is called
	glPushMatrix();
	glScalef(0.53f, 0.53f, 0.53f);	// scale of mars sphere, from the table
	glCallList(v_SphereDL);		// a dl can call another dl that has been previously created
	glPopMatrix();
	glEndList();

	EarthDL = glGenLists(1);
	glNewList(EarthDL, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, EarthTex);	// MarsTex must have already been created when this is called
	glPushMatrix();
	glScalef(0.53f, 0.53f, 0.53f);	// scale of mars sphere, from the table
	glCallList(e_SphereDL);		// a dl can call another dl that has been previously created
	glPopMatrix();
	glEndList();

	JupiterDL = glGenLists(1);
	glNewList(JupiterDL, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, JupiterTex);	// MarsTex must have already been created when this is called
	glPushMatrix();
	glScalef(0.53f, 0.53f, 0.53f);	// scale of mars sphere, from the table
	glCallList(j_SphereDL);		// a dl can call another dl that has been previously created
	glPopMatrix();
	glEndList();

	SaturnDL = glGenLists(1);
	glNewList(SaturnDL, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, SaturnTex);	// MarsTex must have already been created when this is called
	glPushMatrix();
	glScalef(0.53f, 0.53f, 0.53f);	// scale of mars sphere, from the table
	glCallList(s_SphereDL);		// a dl can call another dl that has been previously created
	glPopMatrix();
	glEndList();

	UranusDL = glGenLists(1);
	glNewList(UranusDL, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, UranusTex);	// MarsTex must have already been created when this is called
	glPushMatrix();
	glScalef(0.53f, 0.53f, 0.53f);	// scale of mars sphere, from the table
	glCallList(u_SphereDL);		// a dl can call another dl that has been previously created
	glPopMatrix();
	glEndList();

	NeptuneDL = glGenLists(1);
	glNewList(NeptuneDL, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, NeptuneTex);	// MarsTex must have already been created when this is called
	glPushMatrix();
	glScalef(0.53f, 0.53f, 0.53f);	// scale of mars sphere, from the table
	glCallList(n_SphereDL);		// a dl can call another dl that has been previously created
	glPopMatrix();
	glEndList();

	MercuryDL = glGenLists(1);
	glNewList(MercuryDL, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, MercuryTex);	// MarsTex must have already been created when this is called
	glPushMatrix();
	glScalef(0.53f, 0.53f, 0.53f);	// scale of mars sphere, from the table
	glCallList(me_SphereDL);		// a dl can call another dl that has been previously created
	glPopMatrix();
	glEndList();

	SunDL = glGenLists(1);
	glNewList(SunDL, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, SunTex);	// MarsTex must have already been created when this is called
	glPushMatrix();
	glScalef(0.53f, 0.53f, 0.53f);	// scale of mars sphere, from the table
	glCallList(su_SphereDL);		// a dl can call another dl that has been previously created
	glPopMatrix();
	glEndList();


	// Circles for orbit --------------------------------------------

	Mer_C = glGenLists(1);
	glNewList(Mer_C, GL_COMPILE);
	radius = (0.35f * 0.5) + 1;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; ++i) {
		float theta = 2.0f * 3.1415926f * float(i) / float(100);
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glEndList();

	Ven_C = glGenLists(1);
	glNewList(Ven_C, GL_COMPILE);
	radius = 0.67f * 0.5 + 1;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; ++i) {
		float theta = 2.0f * 3.1415926f * float(i) / float(100);
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glEndList();

	Ear_C = glGenLists(1);
	glNewList(Ear_C, GL_COMPILE);
	radius = 0.92f * 0.5 + 1;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; ++i) {
		float theta = 2.0f * 3.1415926f * float(i) / float(100);
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glEndList();

	Mar_C = glGenLists(1);
	glNewList(Mar_C, GL_COMPILE);
	radius = 1.41f * 0.5 + 1;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; ++i) {
		float theta = 2.0f * 3.1415926f * float(i) / float(100);
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glEndList();

	Jup_C = glGenLists(1);
	glNewList(Jup_C, GL_COMPILE);
	radius = 4.83f * 0.5 + 1;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; ++i) {
		float theta = 2.0f * 3.1415926f * float(i) / float(100);
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glEndList();

	Sat_C = glGenLists(1);
	glNewList(Sat_C, GL_COMPILE);
	radius = 8.90f * 0.5 + 1;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; ++i) {
		float theta = 2.0f * 3.1415926f * float(i) / float(100);
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glEndList();

	Ura_C = glGenLists(1);
	glNewList(Ura_C, GL_COMPILE);
	radius = 17.87f * 0.5 + 1;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; ++i) {
		float theta = 2.0f * 3.1415926f * float(i) / float(100);
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glEndList();

	Nep_C = glGenLists(1);
	glNewList(Nep_C, GL_COMPILE);
	radius = 27.98f * 0.5 + 1;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; ++i) {
		float theta = 2.0f * 3.1415926f * float(i) / float(100);
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glEndList();
	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'w':
		case 'W':
			colorNum = 1;
			break;
		case 'r':
		case 'R':
			colorNum = 2;
			break;
		case 'g':
		case 'G':
			colorNum = 3;
			break;
		case 'b':
		case 'B':
			colorNum = 4;
			break;
		case 'y':
		case 'Y':
			colorNum = 5;
			break;
		case 'p':
		case 'P':
			lightType = 1;
			break;
		case 'o':
		case 'O':
			NowProjection = ORTHO;
			break;
		case 'v':
		case 'V':
			texMode = 1;
			break;
		case 'e':
		case 'E':
			texMode = 2;
			break;
		case 'm':
		case 'M':
			texMode = 3;
			break;
		case 'j':
		case 'J':
			texMode = 4;
			break;
		case 's':
		case 'S':
			texMode = 5;
			break;
		case 'u':
		case 'U':
			texMode = 6;
			break;
		case 'n':
		case 'N':
			texMode = 7;
			break;
		case 't':
		case 'T':
			if (textureMode == 1)
			{
				textureMode = 2;
			}
			else {
				textureMode = 1;
			}
			break;
		case 'l':
		case 'L':
			if (lightingMode == 1)
			{
				lightingMode = 2;
			}
			else {
				lightingMode = 1;
			}
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit( float v[3] )
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
