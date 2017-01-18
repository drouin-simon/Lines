#include "line.h"
#include "IncludeGl.h"
#include "drwDrawingContext.h"

Line::Line() : m_lineWidth( 1.0 )
{
}


Line::~Line() 
{
}


void Line::InternDraw( drwDrawingContext & context )
{
    Vec4 color = m_color * context.m_colorMultiplier;
    glColor4d( color[0], color[1], color[2], color[3] );

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

