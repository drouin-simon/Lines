#ifndef __LinePrimitive_h_
#define __LinePrimitive_h_

#include "Primitive.h"

class Box2d;

class LinePrimitive : public Primitive
{
	
public:
	
	LinePrimitive() {}
	virtual ~LinePrimitive() {}
	
    virtual void StartPoint( double x, double y, double pressure, Box2d & modifBox ) = 0;
    virtual void EndPoint( double x, double y, double pressure, Box2d & modifBox ) = 0;
    virtual void AddPoint( double x, double y, double pressure, Box2d & modifBox ) = 0;
	
};

#endif
