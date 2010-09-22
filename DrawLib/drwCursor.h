#ifndef __drwCursor_h_
#define __drwCursor_h_

#include "SVL.h"

class Circle;
class drwDrawingContext;

class drwCursor
{
	
public:
	
	drwCursor();
	~drwCursor();
	
	void SetRadius( double width );
	void SetPosition( double x, double y );
	void Draw( const drwDrawingContext & context );
	
protected:
	
	Circle * m_circle;
	double m_radius;
	Vec2 m_position;
};

#endif