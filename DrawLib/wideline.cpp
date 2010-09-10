#include "wideline.h"
#include "IncludeGl.h"
#include "math.h"
#include "drwGlslShader.h"
#include "drwDrawingContext.h"

drwGlslShader * WideLine::m_shader = 0;


WideLine::WideLine() 
: m_prevPoint( 0, 0 )
, m_prevPressure( 1.0 ) /*, m_prevDir( 0, 1 ), m_prevLeft( -1, 0 ), m_prevRight( 1, 0 )*/
{
    m_width = 8.0;
    //m_firstPoint = true;
}


WideLine::~WideLine() 
{
}


void WideLine::InternDraw( const drwDrawingContext & context )
{

	// Draw line to texture in grayscale
	if( !m_shader )
		Init();
	m_shader->UseProgram( true );
	m_shader->SetVariable( "margin" , float(.5) );
	
	if( context.m_isOverridingColor )
		glColor4d( context.m_colorOverride[0], context.m_colorOverride[1], context.m_colorOverride[2], context.m_opacity );
	else
		glColor4d( m_color[0] * context.m_opacity, m_color[1] * context.m_opacity, m_color[2] * context.m_opacity, context.m_opacity );
		
	glBlendEquation( GL_MAX );
    glVertexPointer( 2, GL_DOUBLE, 0, m_vertices.GetBuffer() );
	glTexCoordPointer( 3, GL_DOUBLE, 0, m_texCoord.GetBuffer() );
    glDrawElements( GL_QUADS, m_indices.size(), GL_UNSIGNED_INT, m_indices.GetBuffer() );
	glBlendEquation( GL_FUNC_ADD );
	m_shader->UseProgram( false );

	// Paste the texture to screen with the right color
	//if( context.m_isOverridingColor )
		//glColor4d( context.m_colorOverride[0], context.m_colorOverride[1], context.m_colorOverride[2], context.m_opacity );
	//else
		//glColor4d( Color[0], Color[1], Color[2], context.m_opacity );

	// Erase trace on work texture
}


void WideLine::StartPoint( double x, double y, double pressure )
{
	double curWidth = m_width * pressure;
	
	int nextIndex = m_vertices.size();
	m_vertices.push_back( Vec2( x, y ) );
	m_vertices.push_back( Vec2( x, y + curWidth ) );
	m_vertices.push_back( Vec2( x + curWidth, y + curWidth ) );
	m_vertices.push_back( Vec2( x + curWidth, y ) );
	m_vertices.push_back( Vec2( x + curWidth, y - curWidth ) );
	m_vertices.push_back( Vec2( x, y - curWidth ) );
	m_vertices.push_back( Vec2( x - curWidth, y - curWidth ) );
	m_vertices.push_back( Vec2( x - curWidth, y ) );
	m_vertices.push_back( Vec2( x - curWidth, y + curWidth ) );
	
	m_texCoord.push_back( Vec3( 0.0, 0.0, pressure ) );
	m_texCoord.push_back( Vec3( 0.0, 1.0, pressure ) );
	m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
	m_texCoord.push_back( Vec3( 1.0, 0.0, pressure ) );
	m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
	m_texCoord.push_back( Vec3( 0.0, 1.0, pressure ) );
	m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
	m_texCoord.push_back( Vec3( 1.0, 0.0, pressure ) );
	m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
	
	m_indices.push_back( nextIndex );
	m_indices.push_back( nextIndex + 3 );
	m_indices.push_back( nextIndex + 2 );
	m_indices.push_back( nextIndex + 1 );
	
	m_indices.push_back( nextIndex );
	m_indices.push_back( nextIndex + 5 );
	m_indices.push_back( nextIndex + 4 );
	m_indices.push_back( nextIndex + 3 );
	
	m_indices.push_back( nextIndex );
	m_indices.push_back( nextIndex + 7 );
	m_indices.push_back( nextIndex + 6 );
	m_indices.push_back( nextIndex + 5 );
	
	m_indices.push_back( nextIndex );
	m_indices.push_back( nextIndex + 1 );
	m_indices.push_back( nextIndex + 8 );
	m_indices.push_back( nextIndex + 7 );
	
    m_prevPoint[0] = x;
    m_prevPoint[1] = y;
	m_prevPressure = pressure;
}


void WideLine::EndPoint( double x, double y, double pressure )
{
    AddPoint( x, y, pressure );
}


void WideLine::AddPoint( double x, double y, double pressure )
{
	double prevWidth = m_width * m_prevPressure;
	double curWidth = m_width * pressure;
	
	Vec2 newPoint( x, y );
	Vec2 direction = newPoint - m_prevPoint;
	direction.Normalise();
	Vec2 left( -direction[1], direction[0] );
	Vec2 leftOrig = left * prevWidth;
	Vec2 leftEnd = left * curWidth;
	Vec2 rightOrig( leftOrig * -1 );
	Vec2 rightEnd( leftEnd * -1 );
	
	Vec2 newPointLeft = newPoint + leftEnd;
	Vec2 newPointRight = newPoint + rightEnd;
	
	Vec2 midCap = newPoint + direction * curWidth;
	Vec2 leftCap = midCap + leftEnd;
	Vec2 rightCap = midCap + rightEnd;
	
	Vec2 leftStart = m_prevPoint + leftOrig;
	Vec2 rightStart = m_prevPoint + rightOrig;
	
	int nextIndex = m_vertices.size();
	
	m_vertices.push_back( m_prevPoint );
	m_texCoord.push_back( Vec3( 0.0, 0.0, m_prevPressure ) );
	m_vertices.push_back( newPoint );
	m_texCoord.push_back( Vec3( 0.0, 0.0, pressure ) );
	m_vertices.push_back( newPointRight );
	m_texCoord.push_back( Vec3( 1.0, 0.0, pressure ) );
	m_vertices.push_back( rightStart );
	m_texCoord.push_back( Vec3( 1.0, 0.0, m_prevPressure ) );
	m_vertices.push_back( newPointLeft );
	m_texCoord.push_back( Vec3( 1.0, 0.0, pressure ) );
	m_vertices.push_back( leftStart );
	m_texCoord.push_back( Vec3( 1.0, 0.0, m_prevPressure ) );
	m_vertices.push_back( rightCap );
	m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
	m_vertices.push_back( midCap );
	m_texCoord.push_back( Vec3( 0.0, 1.0, pressure ) );
	m_vertices.push_back( leftCap );
	m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
	
	m_indices.push_back( nextIndex );
	m_indices.push_back( nextIndex + 3 );
	m_indices.push_back( nextIndex + 2 );
	m_indices.push_back( nextIndex + 1 );
	
	m_indices.push_back( nextIndex );
	m_indices.push_back( nextIndex + 1 );
	m_indices.push_back( nextIndex + 4 );
	m_indices.push_back( nextIndex + 5 );
	
	m_indices.push_back( nextIndex + 1);
	m_indices.push_back( nextIndex + 2 );
	m_indices.push_back( nextIndex + 6 );
	m_indices.push_back( nextIndex + 7 );
	
	m_indices.push_back( nextIndex + 1);
	m_indices.push_back( nextIndex + 7 );
	m_indices.push_back( nextIndex + 8 );
	m_indices.push_back( nextIndex + 4 );

    // Add a new line segment
    //AddSegment( left, right, newPoint );
    
    // if we have more than 2 points make the joint between th 2 last segments
    //if( !m_firstPoint )
        //AddJoint( left, right, direction );
    //else
        //m_firstPoint = false;

    // cache information for next AddPoint
    m_prevPoint = newPoint;
	m_prevPressure = pressure;
    //m_prevDir   = direction;
    //m_prevLeft  = left;
    //m_prevRight = right;
}

static const char* shaderCode = " \
uniform float margin; \
\
void main() \
{ \
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
		gl_FragColor = gl_Color * fact; \
	} \
	else \
		gl_FragColor = gl_Color * pressure; \
}";


void WideLine::Init()
{
	if( !m_shader)
	{
		m_shader = new drwGlslShader;
		m_shader->AddShaderMemSource( shaderCode );
		m_shader->Init();
	}
}

/*
void WideLine::AddSegment( const Vec2 & left, const Vec2 & right, const Vec2 & newPoint )
{
    int nextIndex = m_vertex.size();
	m_vertex.push_back( m_prevPoint );
    m_vertex.push_back( m_prevPoint + left );
    m_vertex.push_back( m_prevPoint + right );
	m_vertex.push_back( newPoint );
    m_vertex.push_back( newPoint + left );
	m_vertex.push_back( newPoint + right );
	
	m_texCoord.push_back( 1.0 );
	m_texCoord.push_back( 0.0 );
	m_texCoord.push_back( 0.0 );
	m_texCoord.push_back( 1.0 );
	m_texCoord.push_back( 0.0 );
	m_texCoord.push_back( 0.0 );

    m_indices.push_back( nextIndex );
    m_indices.push_back( nextIndex + 4 );
    m_indices.push_back( nextIndex + 1 );
	m_indices.push_back( nextIndex );
    m_indices.push_back( nextIndex + 3 );
    m_indices.push_back( nextIndex + 4 );
	m_indices.push_back( nextIndex );
    m_indices.push_back( nextIndex + 5 );
    m_indices.push_back( nextIndex + 3 );
    m_indices.push_back( nextIndex );
    m_indices.push_back( nextIndex + 2 );
    m_indices.push_back( nextIndex + 5 );
}


void WideLine::AddJoint( const Vec2 & left, const Vec2 & right, const Vec2 & direction )
{
    bool turnLeft = dot( m_prevLeft, direction ) > 0 ? true : false;
    Vec2 newSide = turnLeft ? right : left;
    Vec2 oldSide = turnLeft ? m_prevRight : m_prevLeft;
	
	int nextIndex = m_vertex.size();
	m_vertex.push_back( m_prevPoint );
	m_vertex.push_back( m_prevPoint + newSide );
	m_vertex.push_back( m_prevPoint + oldSide );
	m_texCoord.push_back( 1.0 );
	m_texCoord.push_back( 0.0 );
	m_texCoord.push_back( 0.0 );
	m_indices.push_back( nextIndex );
	m_indices.push_back( nextIndex + 1 );
	m_indices.push_back( nextIndex + 2 );
}*/
