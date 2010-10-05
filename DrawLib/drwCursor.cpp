#include "drwCursor.h"
#include "Circle.h"

drwCursor::drwCursor()
: m_radius( 1.0 )
, m_position( 0.0, 0.0 )
, m_showArrow( false )
, m_percentSide( .5 )
, m_arrowPointSize( .2 )
{
	m_circle = new Circle;
	m_circle->SetFill( false );
	m_circle->SetContour( true );
}

drwCursor::~drwCursor()
{
	delete m_circle;
}

void drwCursor::SetRadius( double radius )
{
	m_radius = radius;
}

void drwCursor::SetPosition( double x, double y )
{
	m_position = Vec2( x, y );
}

void drwCursor::Draw( const drwDrawingContext & context )
{
	glPushMatrix();
	glLoadIdentity();
	glTranslate( m_position );
	glScaled( m_radius, m_radius, 1.0 );
	m_circle->Draw( context );
	if( m_showArrow )
		DrawArrow();
	glPopMatrix();
}

void drwCursor::DrawArrow()
{
	double distLeft = 1.0 + m_percentSide;
	Vec2 arrowTop( -distLeft, 1.0 );
	Vec2 arrowBottom( -distLeft, -1.0 );
	Vec2 topLeft( arrowTop - Vec2( m_arrowPointSize, m_arrowPointSize ) );
	Vec2 topRight( arrowTop - Vec2( -m_arrowPointSize, m_arrowPointSize ) );
	Vec2 bottomRight( arrowBottom + Vec2( m_arrowPointSize, m_arrowPointSize ) );
	Vec2 bottomLeft( arrowBottom + Vec2( -m_arrowPointSize, m_arrowPointSize ) );
	glBegin( GL_LINES );
	{
		glVertex( arrowTop );
		glVertex( arrowBottom );
		glVertex( arrowTop );
		glVertex( topRight );
		glVertex( arrowTop );
		glVertex( topLeft );
		glVertex( arrowBottom );
		glVertex( bottomLeft );
		glVertex( arrowBottom );
		glVertex( bottomRight );
	}
	glEnd();
}