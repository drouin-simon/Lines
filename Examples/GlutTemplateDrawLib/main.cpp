#include <glut.h>
#include <stdlib.h>
#include "drwGlslShader.h"

// original app params
int winWidth = 800;
int winHeight = 600;
int winX = 50;
int winY = 50;
const char appName[] = "Glut Template";
drwGlslShader shaderProgram;
float lineRadius = 1.0;

void display(void)
{
	glClear( GL_COLOR_BUFFER_BIT );
	
	shaderProgram.UseProgram( true );
	shaderProgram.SetVariable( "line_radius", lineRadius );
	
	glColor4d( 1.0, 0.0, 0.0, 0.0 );
	glBegin( GL_QUADS );
	glNormal3f( -.7071, .7071, 0 ); glVertex2d( 50, 50 );
	glNormal3f( .7071, -.7071, 0 ); glVertex2d( 50, 50 );
	glNormal3f( .7071, -.7071, 0 ); glVertex2d( 300, 300 );
	glNormal3f( -.7071, .7071, 0 ); glVertex2d( 300, 300 );
	glEnd();
	
	shaderProgram.UseProgram( false );

    glutSwapBuffers();
}


static const char * pixelShaderCode = " \
uniform float margin; \
uniform bool erase; \
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
		if( !erase ) \
			gl_FragColor[3] = fact; \
		else \
			gl_FragColor[3] = 1.0 - fact; \
	} \
	else \
	{ \
		if( !erase ) \
			gl_FragColor[3] = pressure; \
		else \
			gl_FragColor[3] = 1.0 - pressure; \
	} \
}";

static const char * vertexShaderCode = " \
uniform float line_radius; \
void main() \
{ \
	vec4 scaled_normal = vec4( 0.0, 0.0, 0.0, 0.0 ); \
	scaled_normal.xyz = gl_Normal; \
	scaled_normal *= line_radius; \
	vec4 newVertex = gl_Vertex + scaled_normal; \
	gl_Position = gl_ModelViewProjectionMatrix * newVertex; \
	gl_FrontColor = gl_Color; \
}";


void init(void) 
{
	shaderProgram.AddVertexShaderMemSource( vertexShaderCode );
	bool shaderInit = shaderProgram.Init();
	if( !shaderInit )
		exit( -1 );
	shaderProgram.UseProgram( true );
	shaderProgram.SetVariable( "line_radius", lineRadius );
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
			lineRadius += 1;
			glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT:
		break;
    case GLUT_KEY_RIGHT:
        break;
    case GLUT_KEY_DOWN:
			if( lineRadius > 1 )
				lineRadius -= 1;
			glutPostRedisplay();
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