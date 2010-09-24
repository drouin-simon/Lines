#include "drwCursor.h"
#include "Circle.h"

drwCursor::drwCursor()
: m_radius( 1.0 )
, m_position( 0.0, 0.0 )
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
	glPopMatrix();
}