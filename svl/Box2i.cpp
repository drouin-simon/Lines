#include "Box2i.h"
#include <algorithm>

Box2i::Box2i() : m_xMin( -1 ), m_xMax( -1 ), m_yMin( -1 ), m_yMax( -1 )
{
}

Box2i::Box2i( int xmin, int xmax, int ymin, int ymax )
    : m_xMin( xmin )
    , m_xMax( xmax )
    , m_yMin( ymin )
    , m_yMax( ymax )
{}

Box2i::Box2i( const Box2i & other )
{
    m_xMin = other.m_xMin;
    m_xMax = other.m_xMax;
    m_yMin = other.m_yMin;
    m_yMax = other.m_yMax;
}

Box2i & Box2i::operator=( const Box2i & other )
{
    m_xMin = other.m_xMin;
    m_xMax = other.m_xMax;
    m_yMin = other.m_yMin;
    m_yMax = other.m_yMax;
    return *this;
}

void Box2i::Init( int xMin, int xMax, int yMin, int yMax )
{
    m_xMin = xMin;
    m_xMax = xMax;
    m_yMin = yMin;
    m_yMax = yMax;
}

void Box2i::Reset()
{
    m_xMin = -1;
    m_xMax = -1;
    m_yMin = -1;
    m_yMax = -1;
}

bool Box2i::IsInside( int x, int y ) const
{
    if( x >= m_xMin && x <= m_xMax && y >= m_yMin && y <= m_yMax )
        return true;
    return false;
}	

void Box2i::IncludePoint( int x, int y )
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

bool Box2i::IsEmpty() const
{
    return ( GetWidth() == 0 && GetHeight() == 0 );
}

bool Box2i::Intersect( const Box2i & other ) const
{
    if( IsInside( other.m_xMin, other.m_yMin ) ||
        IsInside( other.m_xMax, other.m_yMin ) ||
        IsInside( other.m_xMin, other.m_yMax ) ||
        IsInside( other.m_xMax, other.m_yMax ) )
        return true;
    return false;
}

bool Box2i::Intersect( const Box2i & b1, const Box2i & b2 )
{
    if( b1.Intersect( b2 ) )
        return true;
    return false;
}	

void Box2i::AdjustBound( const Box2i & other )
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

std::ostream &operator << (std::ostream &s, const Box2i &b )
{
    s << "Origin: ( " << b.XMin() << ", " << b.YMin() << ") ";
    s << "Size: ( " << b.GetWidth() << "," << b.GetHeight() << " )";
    return s;
}
