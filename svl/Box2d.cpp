#include "Box2d.h"
#include <limits>

Box2d::Box2d()
    : m_xMin( std::numeric_limits<double>::max() )
    , m_xMax( std::numeric_limits<double>::min() )
    , m_yMin( std::numeric_limits<double>::max() )
    , m_yMax( std::numeric_limits<double>::min() )
{
}

Box2d::Box2d( double xmin, double xmax, double ymin, double ymax )
    : m_xMin( xmin )
    , m_xMax( xmax )
    , m_yMin( ymin )
    , m_yMax( ymax )
{}

void Box2d::Init()
{
    m_xMin = std::numeric_limits<double>::max();
    m_xMax = std::numeric_limits<double>::min();
    m_yMin = std::numeric_limits<double>::max();
    m_yMax = std::numeric_limits<double>::min();
}

void Box2d::Init( double xmin, double xmax, double ymin, double ymax )
{
    m_xMin = xmin;
    m_yMin = ymin;
    m_xMax = xmax;
    m_yMax = ymax;
}

void Box2d::Reset()
{
    m_xMin = -1.0;
    m_xMax = -1.0;
    m_yMin = -1.0;
    m_yMax = -1.0;
}

bool Box2d::IsInside( double x, double y ) const
{
    return x >= m_xMin && x <= m_xMax && y >= m_yMin && y <= m_yMax;
}	

void Box2d::IncludePoint( double x, double y )
{
    if(x < m_xMin)
        m_xMin = x;
    else if( x > m_xMax )
        m_xMax = x;
    if( y < m_yMin )
        m_yMin = y;
    else if( y > m_yMax )
        m_yMax = y;
}	

bool Box2d::IsEmpty() const
{
    return ( GetWidth() == 0.0 && GetHeight() == 0.0 );
}

bool Box2d::XRangeIntersect( const Box2d & b ) const
{
    return ( m_xMin >= b.m_xMin && m_xMin <= b.m_xMax ) ||
            ( m_xMax >= b.m_xMin && m_xMax <= b.m_xMax ) ||
            ( b.m_xMin >= m_xMin && b.m_xMin <= m_xMax ) ||
            ( b.m_xMax >= m_xMin && b.m_xMax <= m_xMax );
}

bool Box2d::YRangeIntersect( const Box2d & b ) const
{
    return ( m_yMin >= b.m_yMin && m_yMin <= b.m_yMax ) ||
            ( m_yMax >= b.m_yMin && m_yMax <= b.m_yMax ) ||
            ( b.m_yMin >= m_yMin && b.m_yMin <= m_yMax ) ||
            ( b.m_yMax >= m_yMin && b.m_yMax <= m_yMax );
}

// Two boxes intersect if at leas
bool Box2d::Intersect( const Box2d & b ) const
{
    return XRangeIntersect( b ) && YRangeIntersect( b );
}

bool Box2d::Intersect( const Box2d & b1, const Box2d & b2 )
{
    return b1.Intersect( b2 );
}	

void Box2d::AdjustBound( const Box2d & other )
{
    if( !IsEmpty() )
    {
        m_xMin = std::min( m_xMin, other.m_xMin );
        m_xMax = std::max( m_xMax, other.m_xMax );
        m_yMin = std::min( m_yMin, other.m_yMin );
        m_yMax = std::max( m_yMax, other.m_yMax );
    }
    else
        *this = other;
}

std::ostream &operator << (std::ostream &s, const Box2d &b )
{
    s << "Origin: ( " << b.XMin() << ", " << b.YMin() << ") ";
    s << "Size: ( " << b.GetWidth() << "," << b.GetHeight() << " )";
    return s;
}
