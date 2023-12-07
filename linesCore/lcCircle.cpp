#include "lcCircle.h"

Circle::Circle() : Center( Vec2( 0, 0 ) )
{
    Radius = 1.0;
    NumberOfSubdiv = 20;
    Angle1 = 0;
    Angle2 = 2 * vl_pi;
    IntermediatePointOnly = 0;
    Contour = false;
    Fill = true;

    m_engine = new GraphicsEngine();
}

Circle::~Circle()
{
    delete m_engine;
}

void Circle::SetCenter( Vec2 center )
{
    SetCenter( center[0], center[1] );
}

void Circle::SetCenter( double x, double y )
{
    Center[0] = x;
    Center[1] = y;
    MarkModified();
}

void Circle::SetRadius( double r )
{
    Radius = r;
    MarkModified();
}

const Box2d & Circle::BoundingBox()
{
    m_boundingBox.Init( Center[0] - Radius, Center[0] + Radius, Center[1] - Radius, Center[1] + Radius );
    return m_boundingBox;
}

void Circle::InternDraw( drwDrawingContext & context )
{
    // Generate the polygons if not already done
    if( m_poly.IsEmpty() )
    {
        if( Fill )
            GeneratePolygonData();
        if( Contour )
            GenerateContour();
    }

    // Draw the polygons
    m_engine->DrawPolygon(m_poly, m_color);
}

void Circle::GeneratePolygonData()
{
    drwVec2Array & points = m_poly.GetPoints();
    points.push_back( Center );
    GeneratePoints( points );

    drwIndexArray & indicies = m_poly.GetTriangles();
    int numberOfTriangles = points.size() - 2;
    int i;
    for( i = 0; i < numberOfTriangles; i++ )
    {
        indicies.push_back( 0 );
        indicies.push_back( i + 1 );
        indicies.push_back( i + 2 );
    }
}


void Circle::GenerateContour()
{
    drwVec2Array & points = m_poly.GetPoints();
    GeneratePoints( points );

    drwIndexArray & indices = m_poly.GetLines();
    int i;
    int numberOfPoints = points.size();
    for( i = 0; i < numberOfPoints; i++ )
    {
        indices.push_back( i );
    }
}

void Circle::GeneratePoints( drwVec2Array & points )
{
    double step = 2 * vl_pi / NumberOfSubdiv;
    int numberOfPoints = (int) ceil( (Angle2 - Angle1) / step );

    Vec2 newVertex(0,0);
    int i = IntermediatePointOnly ? 1 : 0;
    double angle = 0;
    for( ; i < numberOfPoints; i++ )
    {
        angle = Angle1 + i * step;
        newVertex[0] = cos( angle );
        newVertex[1] = sin( angle );
        newVertex *= Radius;
        newVertex += Center;
        points.push_back( newVertex );
    }
    if( !IntermediatePointOnly )
    {
        newVertex[0] = cos( Angle2 );
        newVertex[1] = sin( Angle2 );
        newVertex *= Radius;
        newVertex += Center;
        points.push_back( newVertex );
    }
}

void Circle::MarkModified()
{
    m_poly.Clear();
}
