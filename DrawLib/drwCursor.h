#ifndef __drwCursor_h_
#define __drwCursor_h_

class drwLineTool;
class drwDrawingWidget;
class QPainter;

class drwCursor
{
	
public:
	
    drwCursor( drwLineTool * m_lineTool, drwDrawingWidget * widget );
	~drwCursor();
	
    void SetPosition( int x, int y );
    void Draw( QPainter & painter );
	
protected:
	
    drwLineTool * m_lineTool;
    drwDrawingWidget * m_drawingWidget;

    int m_position[2];
};

#endif
