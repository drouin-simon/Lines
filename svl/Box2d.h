#ifndef __Box2d_h_
#define __Box2d_h_

class Box2d
{

public:

    Box2d();
    Box2d( double xmin, double xmax, double ymin, double ymax );

    void Init();
    void Init( double xmin, double xmax, double ymin, double ymax );
    void Reset();

    //  determine if (x,y) is inside the box
    bool IsInside( double x, double y ) const;

    //  ajust box (if needed) so that (x,y) is inside
    void IncludePoint( double x, double y );

    double XMin() const { return m_xMin; }
    double YMin() const { return m_yMin; }
    double XMax() const { return m_xMax; }
    double YMax() const { return m_yMax; }
    double GetWidth() const { return m_xMax - m_xMin; }
    double GetHeight() const { return m_yMax - m_yMin; }
    bool IsEmpty() const;

    bool XRangeIntersect( const Box2d & b ) const;
    bool YRangeIntersect( const Box2d & b ) const;
    bool Intersect( const Box2d & other ) const;
    static bool Intersect( const Box2d & b1, const Box2d & b2 );

    void AdjustBound( const Box2d & other );

private:

    double m_xMin;
    double m_xMax;
    double m_yMin;
    double m_yMax;

};

#include <ostream>
std::ostream &operator << (std::ostream &s, const Box2d &b );

#endif
