#ifndef __Box2i_h_
#define __Box2i_h_

class Box2i
{

public:

    Box2i();
    Box2i( int xmin, int xmax, int ymin, int ymax );
    Box2i( const Box2i & other );
    Box2i & operator=( const Box2i & other );

    void Init( int xMin, int xMax, int yMin, int yMax );
    void Reset();

    //  determine if (x,y) is inside the box
    bool IsInside( int x, int y ) const;

    //  ajust box (if needed) so that (x,y) is inside
    void IncludePoint( int x, int y );
    int XMin() const { return m_xMin; }
    int YMin() const { return m_yMin; }
    int XMax() const { return m_xMax; }
    int YMax() const { return m_yMax; }
    int GetWidth() const { return m_xMax - m_xMin; }
    int GetHeight() const { return m_yMax - m_yMin; }
    bool IsEmpty() const;

    bool Intersect( const Box2i & other ) const;
    static bool Intersect( const Box2i & b1, const Box2i & b2 );
    void AdjustBound( const Box2i & other );

private:

    int m_xMin;
    int m_xMax;
    int m_yMin;
    int m_yMax;

};

#include <ostream>
std::ostream &operator << ( std::ostream &s, const Box2i &b );

#endif
