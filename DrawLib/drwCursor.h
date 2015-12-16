#ifndef __drwCursor_h_
#define __drwCursor_h_

#include <QObject>
#include <QRect>

class drwLineTool;
class drwDrawingWidget;
class QPainter;

class drwCursor : public QObject
{
    Q_OBJECT
	
public:
	
    drwCursor( drwLineTool * m_lineTool, drwDrawingWidget * widget );
	~drwCursor();
	
    void SetPosition( int x, int y );
    void SetColor( QString colorName ) { m_colorName = colorName; }
    void Draw( QPainter & painter );

signals:

    void Modified();
	
protected:
	
    drwLineTool * m_lineTool;
    drwDrawingWidget * m_drawingWidget;

    QString m_colorName;
    int m_position[2];
};

#endif
