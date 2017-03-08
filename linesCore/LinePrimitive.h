#ifndef __LinePrimitive_h_
#define __LinePrimitive_h_

#include "Primitive.h"

class LinePrimitive : public Primitive
{
	
public:
	
    LinePrimitive() : m_boundingBox( 0.0, 0.0, 0.0, 0.0 ) {}
	virtual ~LinePrimitive() {}
	
    virtual void StartPoint( double x, double y, double pressure ) = 0;
    virtual void EndPoint( double x, double y, double pressure ) = 0;
    virtual void AddPoint( double x, double y, double pressure ) = 0;

    const Box2d & BoundingBox() { return m_boundingBox; }

protected:

    Box2d m_boundingBox;
	
};

#endif
