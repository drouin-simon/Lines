#include <IncludeGlut.h>
#include <stdlib.h>
#include "drwGlslShader.h"
#include <sstream>
#include "SVL.h"

// original app params
int winWidth = 800;
int winHeight = 600;
int winX = 50;
int winY = 50;
const char appName[] = "Glut Template";
drwGlslShader shaderProgram;
float lineRadius = 1.0;

Vec2 point1( 50.0, 50.0 );
Vec2 point2( 300.0, 300.0 );

void RenderString( float x, float y, const char * string )
{
    glColor3f( 1.0, 1.0, 1.0 );
    glRasterPos2f( x, y );
    const char * it = string;
    while( *it != 0 )
    {
        glutBitmapCharacter( GLUT_BITMAP_9_BY_15, *it );
        ++it;
    }
}

void display(void)
{
	glClear( GL_COLOR_BUFFER_BIT );
	
	shaderProgram.UseProgram( true );
	shaderProgram.SetVariable( "line_radius", lineRadius );
    shaderProgram.SetVariable( "min_line_radius", float(4.0) );
    shaderProgram.SetVariable( "pix_margin", float(4.0) );

    Vec2 diff = point2 - point1;
    diff.Normalise();
    Vec2 front = diff;
    Vec2 back = -1 * diff;
    Vec2 left( -diff[1], diff[0] );
    Vec2 right = -1.0 * left;
    Vec2 backRight = back + right;
    Vec2 backLeft = back + left;
    Vec2 frontRight = front + right;
    Vec2 frontLeft = front + left;
	
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glColor4d( 1.0, 0.0, 0.0, 0.0 );
	glBegin( GL_QUADS );
    {
        // Start right
        glTexCoord3f( 0, 1.0, 1.0 );
        glNormal3f( back[0], back[1], 0 );              glVertex2d( point1[0], point1[1] );
        glTexCoord3f( 1.0, 1.0, 1.0 );
        glNormal3f( backRight[0], backRight[1], 0 );    glVertex2d( point1[0], point1[1] );
        glTexCoord3f( 1.0, 0.0, 1.0 );
        glNormal3f( right[0], right[1], 0 );            glVertex2d( point1[0], point1[1] );
        glTexCoord3f( 0, 0.0, 1.0 );
        glNormal3f( 0, 0, 0 );                          glVertex2d( point1[0], point1[1] );

        // Start left
        glTexCoord3f( 0.0, 1.0, 1.0 );
        glNormal3f( back[0], back[1], 0 );              glVertex2d( point1[0], point1[1] );
        glTexCoord3f( 0.0, 0.0, 1.0 );
        glNormal3f( 0, 0, 0 );                          glVertex2d( point1[0], point1[1] );
        glTexCoord3f( 1.0, 0.0, 1.0 );
        glNormal3f( left[0], left[1], 0 );              glVertex2d( point1[0], point1[1] );
        glTexCoord3f( 1.0, 1.0, 1.0 );
        glNormal3f( backLeft[0], backLeft[1], 0 );      glVertex2d( point1[0], point1[1] );

        // Center right
        glTexCoord3f( 0.0, 0.0, 1.0 );
        glNormal3f( 0, 0, 0 );                          glVertex2d( point1[0], point1[1] );
        glTexCoord3f( 1.0, 0.0, 1.0 );
        glNormal3f( right[0], right[1], 0 );            glVertex2d( point1[0], point1[1] );
        glTexCoord3f( 1.0, 0.0, 1.0 );
        glNormal3f( right[0], right[1], 0 );            glVertex2d( point2[0], point2[1] );
        glTexCoord3f( 0, 0.0, 1.0 );
        glNormal3f( 0, 0, 0 );                          glVertex2d( point2[0], point2[1] );

        // Center left
        glTexCoord3f( 1.0, 0.0, 1.0 );
        glNormal3f( left[0], left[1], 0 );              glVertex2d( point1[0], point1[1] );
        glTexCoord3f( 0.0, 0.0, 1.0 );
        glNormal3f( 0, 0, 0 );                          glVertex2d( point1[0], point1[1] );
        glTexCoord3f( 0, 0.0, 1.0 );
        glNormal3f( 0, 0, 0 );                          glVertex2d( point2[0], point2[1] );
        glTexCoord3f( 1.0, 0.0, 1.0 );
        glNormal3f( left[0], left[1], 0 );              glVertex2d( point2[0], point2[1] );

        // end right
        glTexCoord3f( 0, 0.0, 1.0 );
        glNormal3f( 0, 0, 0 );                          glVertex2d( point2[0], point2[1] );
        glTexCoord3f( 1.0, 0.0, 1.0 );
        glNormal3f( right[0], right[1], 0 );            glVertex2d( point2[0], point2[1] );
        glTexCoord3f( 1.0, 1.0, 1.0 );
        glNormal3f( frontRight[0], frontRight[1], 0 );  glVertex2d( point2[0], point2[1] );
        glTexCoord3f( 0.0, 1.0, 1.0 );
        glNormal3f( front[0], front[1], 0 );            glVertex2d( point2[0], point2[1] );

        // end left
        glTexCoord3f( 0, 0.0, 1.0 );
        glNormal3f( 0, 0, 0 );                          glVertex2d( point2[0], point2[1] );
        glTexCoord3f( 0, 1.0, 1.0 );
        glNormal3f( front[0], front[1], 0 );            glVertex2d( point2[0], point2[1] );
        glTexCoord3f( 1.0, 1.0, 1.0 );
        glNormal3f( frontLeft[0], frontLeft[1], 0 );    glVertex2d( point2[0], point2[1] );
        glTexCoord3f( 1.0, 0.0, 1.0 );
        glNormal3f( left[0], left[1], 0 );              glVertex2d( point2[0], point2[1] );
    }
	glEnd();
	
	shaderProgram.UseProgram( false );

    std::stringstream s;
    s << "line radius: " << lineRadius;
    RenderString( 300, 20, s.str().c_str() );

    glutSwapBuffers();
}


static const char * pixelShaderCode = " \
varying float margin; \
\
void main() \
{ \
	gl_FragColor = gl_Color; \
	float s = gl_TexCoord[0].x; \
	float t = gl_TexCoord[0].y; \
	float pressure = gl_TexCoord[0].z; \
	float r = sqrt( s*s + t*t ); \
	float invR = 1.0 - r; \
	if( invR < margin ) \
	{ \
		float x = 1.0 - invR / margin; \
		float exponent = - ( x * x / 0.2 );\
		float fact = exp( exponent ); \
		fact = fact * pressure; \
        gl_FragColor[3] = fact; \
	} \
	else \
	{ \
        gl_FragColor[3] = pressure; \
	} \
}";

static const char * vertexShaderCode = " \
uniform float line_radius; \
uniform float min_line_radius; \
uniform float pix_margin; \
varying float margin; \
void main() \
{ \
	vec4 scaled_normal = vec4( 0.0, 0.0, 0.0, 0.0 ); \
	scaled_normal.xyz = gl_Normal; \
    float radius = line_radius; \
    float damp_factor = 1.0; \
    if( radius < min_line_radius ) \
    { \
        radius = min_line_radius; \
        damp_factor = line_radius / min_line_radius; \
    } \
    margin = pix_margin / radius; \
    scaled_normal *= radius; \
	vec4 newVertex = gl_Vertex + scaled_normal; \
	gl_Position = gl_ModelViewProjectionMatrix * newVertex; \
    gl_FrontColor = gl_Color * damp_factor; \
    gl_TexCoord[0] = gl_MultiTexCoord0; \
}";


void init(void) 
{
    shaderProgram.AddShaderMemSource( pixelShaderCode );
	shaderProgram.AddVertexShaderMemSource( vertexShaderCode );
	bool shaderInit = shaderProgram.Init();
	if( !shaderInit )
		exit( -1 );
	shaderProgram.UseProgram( true );
	shaderProgram.SetVariable( "line_radius", lineRadius );
    shaderProgram.SetVariable( "min_line_radius", float(.5) );
    shaderProgram.SetVariable( "pix_margin", float(2.0) );
	shaderProgram.UseProgram( false );
	
	glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f);
   //glShadeModel ( GL_SMOOTH );
   glShadeModel ( GL_FLAT );

   // enable texturing
   //glEnable( GL_TEXTURE_2D );

}


void reshape(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport ( 0, 0, w, h );   
	gluOrtho2D( 0, w, 0, h );
}


void mouse(int button, int state, int x, int y) 
{
   switch (button) 
   {
   case GLUT_LEFT_BUTTON:
       point2 = Vec2( x, winHeight - y - 1 );
     break;
   }
   glutPostRedisplay();
}


void keyboard (unsigned char key, int x, int y)
{
   switch (key) 
   {
   case 27:
	   exit(0);
	   break;
	}
   glutPostRedisplay();
}


void special(int key, int x, int y)
{
    switch (key) 
    {
    case GLUT_KEY_UP:
        {
            if( lineRadius >= 1.0 )
                lineRadius += 1.0;
            else
                lineRadius *= 1.5;
			glutPostRedisplay();
        }
        break;
    case GLUT_KEY_LEFT:
		break;
    case GLUT_KEY_RIGHT:
        break;
    case GLUT_KEY_DOWN:
        {
            if( lineRadius >= 2.0 )
                lineRadius -= 1.0;
            else
                lineRadius *= .75;
			glutPostRedisplay();
        }
        break;
    }
}


void special_up(int key, int x, int y)
{
   switch (key) 
    {
    case GLUT_KEY_UP:
        break;
    case GLUT_KEY_LEFT:
        break;
    case GLUT_KEY_RIGHT:
        break;
    case GLUT_KEY_DOWN:
        break;
    }
}


//void Idle(void)
//{
//}
   
int main(int argc, char** argv)
{
	glutInit( &argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize (winWidth, winHeight); 
	glutInitWindowPosition (winX, winY);
	glutCreateWindow (appName);
	init ();
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape); 
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	//glutIdleFunc(Idle);
    glutSpecialFunc( special );
    glutSpecialUpFunc( special_up );
	//glutFullScreen();
	glutMainLoop();
	return 0;
}
