#ifndef __Primitive_h_
#define __Primitive_h_

#include <SVL.h>

class Vec3;
class drwSWFWriter;
class drwDrawingContext;

class Primitive
{

public:
	
	Primitive() : m_color(1.0,1.0,1.0) {}
	virtual ~Primitive() {}

	void Draw( const drwDrawingContext & context )
	{
		InternDraw( context );
	}

	void SetColor( const Vec3 & col ) { m_color = col; }
	
protected:
	
	virtual void InternDraw( const drwDrawingContext & context ) = 0;

	Vec3 m_color;

};


#endif
