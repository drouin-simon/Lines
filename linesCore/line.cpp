#include "line.h"
#include "drwDrawingContext.h"

Line::Line() : m_lineWidth( 1.0 )
{
    m_engine = new OpenGLGraphicsEngine();
}


Line::~Line() 
{
    delete m_engine;
}


void Line::InternDraw( drwDrawingContext & context )
{
    Vec4 color = m_color * context.m_colorMultiplier;
    m_engine->DrawLine(m_pointTable.GetBuffer(), m_pointIndex.GetBuffer(), m_pointIndex.size(), m_lineWidth, color);
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

