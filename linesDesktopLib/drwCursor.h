#ifndef __drwCursor_h_
#define __drwCursor_h_

#include <QString>

class drwLineTool;
class drwDrawingWidget;
class QPainter;

class drwCursor
{
	
public:
	
    drwCursor( drwLineTool * m_lineTool, drwDrawingWidget * widget );
	~drwCursor();
	
    void SetPosition( int x, int y );
    void SetColor( QString colorName ) { m_colorName = colorName; }
    void Draw( QPainter & painter );
	
protected:

    double ComputeBaseWidthPix();
    void ComputeCursorRect( double baseWidth, int pos[2], int rect[4] );
	
    drwLineTool * m_lineTool;
    drwDrawingWidget * m_drawingWidget;

    QString m_colorName;
    int m_position[2];
    int m_lastDrawnRect[4];
};

#endif
