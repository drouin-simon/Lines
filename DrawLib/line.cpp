#include "line.h"
#include "SVLgl.h"
#include "drwDrawingContext.h"

Line::Line() : m_lineWidth( 1.0 )
{
}


Line::~Line() 
{
}


void Line::InternDraw( const drwDrawingContext & context )
{
	if( context.m_isOverridingColor )
		glColor4d( context.m_colorOverride[0], context.m_colorOverride[1], context.m_colorOverride[2], context.m_opacity );
	else
		glColor4d( m_color[0], m_color[1], m_color[2], context.m_opacity );

	glLineWidth( m_lineWidth );
    glVertexPointer( 2, GL_DOUBLE, 0, m_pointTable.GetBuffer() );
    glDrawElements(GL_LINE_STRIP, m_pointIndex.size(), GL_UNSIGNED_INT, m_pointIndex.GetBuffer() );
}


void Line::StartPoint( double x, double y, double pressure )
{
    Line::AddPoint( x, y, pressure );
}


void Line::EndPoint( double x, double y, double pressure )
{
    Line::AddPoint( x, y, pressure );
}


void Line::AddPoint( double x, double y, double pressure )
{
    m_pointTable.push_back( Vec2( x, y ) );
    m_pointIndex.push_back( m_pointIndex.size() );
}

