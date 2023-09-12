#include "wideline.h"
#include "IncludeGlew.h"
#include "IncludeGl.h"
#include "math.h"
#include "drwGlslShader.h"
#include "drwDrawingContext.h"
#include "drwDrawableTexture.h"

static const double squareRootOfTwo = sqrt(2.0);
float WideLine::m_pix_margin = 4.0;
float WideLine::m_pix_damp_width = 1.0;
float WideLine::m_sigma_large = 0.4;
float WideLine::m_sigma_small = 0.1;

WideLine::WideLine( double width )
: m_width( width )
, m_pressureWidth( true )
, m_pressureOpacity( true )
, m_fill( false )
, m_erase( false )
, m_prevPoint( 0, 0 )
, m_prevPressure( 1.0 )
, m_doneAddingPoints( false )
{
}


WideLine::~WideLine() 
{
}

void WideLine::InternDraw( drwDrawingContext & context )
{
    // Don't draw if the area of the screen to repaint doesn't intersect
    // this line's bounding box
    if( !m_boundingBox.Intersect( context.DrawingRect() ) )
        return;

	// Draw mask to texture
    drwDrawableTexture * tex = context.GetWorkingTexture();
    tex->DrawToTexture( true );
    glColor4d( 1.0, 1.0, 1.0, 1.0 );

	// 1 - fill if needed
	if( m_fill && m_doneAddingPoints && m_fillIndices.size() > 3 )
	{
		glDisable( GL_BLEND );
		glEnable( GL_COLOR_LOGIC_OP );
		glLogicOp( GL_INVERT );
		glVertexPointer( 2, GL_DOUBLE, 0, m_fillVertices.GetBuffer() );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDrawElements( GL_TRIANGLE_FAN, m_fillIndices.size(), GL_UNSIGNED_INT, m_fillIndices.GetBuffer() );

		glDisable( GL_COLOR_LOGIC_OP );
	}

	// 2 - draw wideline
	glEnable( GL_BLEND );

    drwGlslShader * shader = context.GetWidelineShader();
    if( !shader )
    {
        InitShader( context );
        shader = context.GetWidelineShader();
    }

    shader->UseProgram( true );
    shader->SetVariable( "pressure_width", m_pressureWidth );
    shader->SetVariable( "pressure_opacity", m_pressureOpacity );
    shader->SetVariable( "base_width", float(m_width) );
    shader->SetVariable( "pix_per_unit", float(context.PixelsPerUnit()) );
    shader->SetVariable( "pix_margin", m_pix_margin );
    shader->SetVariable( "pix_damp_width", m_pix_damp_width );
    shader->SetVariable( "sigma_large", m_sigma_large );
    shader->SetVariable( "sigma_small", m_sigma_small );
		
	glBlendEquation( GL_MAX );

    glVertexPointer( 2, GL_DOUBLE, 0, m_vertices.GetBuffer() );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 3, GL_DOUBLE, 0, m_texCoord.GetBuffer() );
    glEnableClientState( GL_NORMAL_ARRAY );
    glNormalPointer( GL_DOUBLE, 0, m_normals.GetBuffer() );
    glDrawElements( GL_QUADS, m_indices.size(), GL_UNSIGNED_INT, m_indices.GetBuffer() );

    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_NORMAL_ARRAY );
    shader->UseProgram( false );

    tex->DrawToTexture( false );

	// Paste the texture to screen with the right color
    if( !m_erase )
    {
        glBlendEquation( GL_FUNC_ADD );
        glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE );
        Vec4 col = m_color * context.m_colorMultiplier;
        glColor4d( col[0], col[1], col[2], col[3] );
    }
    else
    {
        glBlendEquationSeparate( GL_FUNC_ADD, GL_FUNC_REVERSE_SUBTRACT );
        glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE );
        glColor4d( 0.0, 0.0, 0.0, 1.0 );
    }

    int xWinMin = 0;
    int yWinMin = 0;
    context.WorldToGLFrame( m_boundingBox.XMin(), m_boundingBox.YMin(), xWinMin, yWinMin );
    int xWinMax = 1;
    int yWinMax = 1;
    context.WorldToGLFrame( m_boundingBox.XMax(), m_boundingBox.YMax(), xWinMax, yWinMax );
    int width = xWinMax - xWinMin + 1;
    int height = yWinMax - yWinMin + 1;
    tex->PasteToScreen( xWinMin, yWinMin, width, height );

	// Erase trace on work texture
    glBlendEquation( GL_FUNC_ADD );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    tex->DrawToTexture( true );
    tex->Clear( xWinMin, yWinMin, width, height );
    tex->DrawToTexture( false );
}


void WideLine::StartPoint( double x, double y, double pressure )
{
    double curWidth = m_width;
    if( m_pressureWidth )
        curWidth *= pressure;

    // Init bounding box
    m_boundingBox.Init( x - curWidth, x + curWidth, y - curWidth, y + curWidth );

    Vec2 newPoint( x, y );

    // top left (0)
    m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
    m_normals.push_back( Vec3( -1.0, 1.0, 0 ) );
    m_vertices.push_back( newPoint );

    // top center (1)
    m_texCoord.push_back( Vec3( 0.0, 1.0, pressure ) );
    m_normals.push_back( Vec3( 0.0, 1.0, 0 ) );
    m_vertices.push_back( newPoint );

    // top right (2)
    m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
    m_normals.push_back( Vec3( 1.0, 1.0, 0 ) );
    m_vertices.push_back( newPoint );

    // middle left (3)
    m_texCoord.push_back( Vec3( 1.0, 0.0, pressure ) );
    m_normals.push_back( Vec3( -1.0, 0.0, 0 ) );
    m_vertices.push_back( newPoint );

    // middle center (4)
    m_texCoord.push_back( Vec3( 0.0, 0.0, pressure ) );
    m_normals.push_back( Vec3( 0.0, 0.0, 0 ) );
    m_vertices.push_back( newPoint );

    // middle right (5)
    m_texCoord.push_back( Vec3( 1.0, 0.0, pressure ) );
    m_normals.push_back( Vec3( 1.0, 0.0, 0 ) );
    m_vertices.push_back( newPoint );

    // bottom left (6)
    m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
    m_normals.push_back( Vec3( -1.0, -1.0, 0 ) );
    m_vertices.push_back( newPoint );

    // bottom middle (7)
    m_texCoord.push_back( Vec3( 0.0, 1.0, pressure ) );
    m_normals.push_back( Vec3( 0.0, -1.0, 0 ) );
    m_vertices.push_back( newPoint );

    // bottom right (8)
    m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
    m_normals.push_back( Vec3( 1.0, -1.0, 0 ) );
    m_vertices.push_back( newPoint );
	
    m_indices.push_back( 4 );
    m_indices.push_back( 5 );
    m_indices.push_back( 2 );
    m_indices.push_back( 1 );
	
    m_indices.push_back( 3 );
    m_indices.push_back( 4 );
    m_indices.push_back( 1 );
    m_indices.push_back( 0 );
	
    m_indices.push_back( 7 );
    m_indices.push_back( 8 );
    m_indices.push_back( 5 );
    m_indices.push_back( 4 );
	
    m_indices.push_back( 6 );
    m_indices.push_back( 7 );
    m_indices.push_back( 4 );
    m_indices.push_back( 3 );

	// Fill
	if( m_fill )
		AddFillPoint( x, y );

    m_prevPoint[0] = x;
    m_prevPoint[1] = y;
	m_prevPressure = pressure;
}


void WideLine::EndPoint( double x, double y, double pressure )
{
    AddPoint( x, y, pressure );
	if( m_fill )
	{
        AddLinePoint( m_fillVertices.at(0)[0], m_fillVertices.at(0)[1], pressure );
		m_fillIndices.push_back( 0 );
		m_doneAddingPoints = true;
	}
}

void WideLine::AddPoint( double x, double y, double pressure )
{
    AddLinePoint( x, y, pressure );
	if( m_fill )
		AddFillPoint( x, y );
}

void WideLine::AddLinePoint( double x, double y, double pressure )
{
	double curWidth = m_width;
	if( m_pressureWidth )
		curWidth *= pressure;

    // add current point bounding box to global bounding box
	double boxOffset = curWidth * squareRootOfTwo;
	m_boundingBox.IncludePoint( x - boxOffset, y - boxOffset );
	m_boundingBox.IncludePoint( x + boxOffset, y + boxOffset );
	
	Vec2 newPoint( x, y );
	int nextIndex = m_vertices.size();

    Vec2 diff = newPoint - m_prevPoint;
    diff.Normalise();
    Vec2 front = diff;
    Vec2 left( -diff[1], diff[0] );
    Vec2 right = -1.0 * left;
    Vec2 frontRight = front + right;
    Vec2 frontLeft = front + left;

    // start middle (0)
    m_texCoord.push_back( Vec3( 0.0, 0.0, m_prevPressure ) );
    m_normals.push_back( Vec3( 0, 0, 0 ) );
    m_vertices.push_back( m_prevPoint );

    // start right (1)
    m_texCoord.push_back( Vec3( 1.0, 0.0, m_prevPressure ) );
    m_normals.push_back( Vec3( right[0], right[1], 0 ) );
    m_vertices.push_back( m_prevPoint );

    // end right (2)
    m_texCoord.push_back( Vec3( 1.0, 0.0, pressure ) );
    m_normals.push_back( Vec3( right[0], right[1], 0 ) );
    m_vertices.push_back( newPoint );

    // end middle (3)
    m_texCoord.push_back( Vec3( 0, 0.0, pressure ) );
    m_normals.push_back( Vec3( 0, 0, 0 ) );
    m_vertices.push_back( newPoint );

    // start left (4)
    m_texCoord.push_back( Vec3( 1.0, 0.0, m_prevPressure ) );
    m_normals.push_back( Vec3( left[0], left[1], 0 ) );
    m_vertices.push_back( m_prevPoint );

    // end left (5)
    m_texCoord.push_back( Vec3( 1.0, 0.0, pressure ) );
    m_normals.push_back( Vec3( left[0], left[1], 0 ) );
    m_vertices.push_back( newPoint );

    // cap right (6)
    m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
    m_normals.push_back( Vec3( frontRight[0], frontRight[1], 0 ) );
    m_vertices.push_back( newPoint );

    // cap middle (7)
    m_texCoord.push_back( Vec3( 0.0, 1.0, pressure ) );
    m_normals.push_back( Vec3( front[0], front[1], 0 ) );
    m_vertices.push_back( newPoint );

    // cap left (8)
    m_texCoord.push_back( Vec3( 1.0, 1.0, pressure ) );
    m_normals.push_back( Vec3( frontLeft[0], frontLeft[1], 0 ) );
    m_vertices.push_back( newPoint );
	
	m_indices.push_back( nextIndex );
    m_indices.push_back( nextIndex + 1 );
    m_indices.push_back( nextIndex + 2 );
    m_indices.push_back( nextIndex + 3 );
	
	m_indices.push_back( nextIndex );
    m_indices.push_back( nextIndex + 3 );
    m_indices.push_back( nextIndex + 5 );
    m_indices.push_back( nextIndex + 4 );
	
    m_indices.push_back( nextIndex + 3 );
	m_indices.push_back( nextIndex + 2 );
	m_indices.push_back( nextIndex + 6 );
	m_indices.push_back( nextIndex + 7 );
	
    m_indices.push_back( nextIndex + 3);
    m_indices.push_back( nextIndex + 7 );
	m_indices.push_back( nextIndex + 8 );
    m_indices.push_back( nextIndex + 5 );

    // cache information for next AddPoint
    m_prevPoint = newPoint;
	m_prevPressure = pressure;
}

void WideLine::AddFillPoint( double x, double y )
{
	m_fillVertices.push_back( Vec2( x, y ) );
	int lastIndex = m_fillVertices.size() - 1;
	m_fillIndices.push_back( lastIndex );
}

static const char * pixelShaderCode = " \
varying float margin; \
varying float sigma; \
\
void main() \
{ \
    gl_FragColor = gl_Color; \
    float s = gl_TexCoord[0].x; \
    float t = gl_TexCoord[0].y; \
    float r = sqrt( s*s + t*t ); \
    float invR = 1.0 - r; \
    if( invR < margin ) \
    { \
        float x = 1.0 - invR / margin; \
        float exponent = - ( x * x / sigma );\
        float fact = exp( exponent ); \
        gl_FragColor[3] *= fact; \
    } \
}";

static const char * vertexShaderCode = " \
uniform bool pressure_width; \
uniform bool pressure_opacity; \
uniform float base_width; \
uniform float pix_per_unit; \
uniform float pix_margin; \
uniform float pix_damp_width; \
uniform float sigma_large; \
uniform float sigma_small; \
varying float margin; \
varying float sigma; \
void main() \
{ \
    float pressure = gl_MultiTexCoord0[2]; \
    float base_width_pressure = base_width; \
    if( pressure_width ) \
        base_width_pressure *= pressure; \
    float pix_width = base_width_pressure * pix_per_unit; \
    sigma = sigma_large; \
    float damp_factor = 1.0; \
    margin = pix_margin / pix_width; \
    \
    if( pix_width < pix_margin )  \
    { \
        margin = 1.0; \
        if( pix_width < pix_damp_width ) \
        { \
            sigma = sigma_small; \
            damp_factor = pix_width / pix_damp_width; \
        } \
        else \
        { \
            float ratio = ( pix_width - pix_damp_width ) / ( pix_margin - pix_damp_width ); \
            sigma = sigma_small + ratio * ( sigma_large - sigma_small ); \
        } \
        \
        pix_width = pix_margin; \
    } \
    \
    float width = pix_width / pix_per_unit; \
    vec4 scaled_normal = vec4( 0.0, 0.0, 0.0, 0.0 ); \
    scaled_normal.xyz = gl_Normal; \
    scaled_normal *= width; \
    vec4 newVertex = gl_Vertex + scaled_normal; \
    gl_Position = gl_ModelViewProjectionMatrix * newVertex; \
    gl_FrontColor = gl_Color; \
    if( pressure_opacity ) \
        gl_FrontColor[3] *= pressure; \
    gl_FrontColor[3] *= damp_factor; \
    gl_TexCoord[0] = gl_MultiTexCoord0; \
}";

void WideLine::InitShader( drwDrawingContext & context )
{
    drwGlslShader * shader = new drwGlslShader;
    shader->AddShaderMemSource( pixelShaderCode );
    shader->AddVertexShaderMemSource( vertexShaderCode );
    shader->Init();
    context.SetWidelineShader( shader );
}
