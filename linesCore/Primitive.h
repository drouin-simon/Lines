#ifndef __Primitive_h_
#define __Primitive_h_

#include <SVL.h>

class drwDrawingContext;

class Primitive
{

    enum RenderState{ Done, Active, Dirty };

public:
	
	Primitive() : m_color(1.0,1.0,1.0,1.0) {}
	virtual ~Primitive() {}

    void Draw( drwDrawingContext & context )
	{
		InternDraw( context );
	}

	void SetColor( const Vec4 & col ) { m_color = col; }
    void SetRenderState( RenderState state ) { m_renderState = state; }
	
protected:
	
    virtual void InternDraw( drwDrawingContext & context ) = 0;

	Vec4 m_color;
    RenderState m_renderState;

};


#endif
