#ifndef __drwLineToolViewportWidget_h_
#define __drwLineToolViewportWidget_h_

#include <QRect>

class drwDrawingWidget;
class drwLineTool;
class QPainter;

class drwLineToolViewportWidget
{
	
public:
	
    drwLineToolViewportWidget( drwDrawingWidget * w, drwLineTool * lt );
	~drwLineToolViewportWidget();
	
    void Activate( int x, int y );
    void Deactivate();

    bool MousePress( int x, int y );
    bool MouseRelease( int x, int y );
    bool MouseMove( int x, int y );
	void Draw( QPainter & painter );
	
protected:

    void ModifyColor( int x );
    double PosToSliderValue( int x, QRect & r );
    int SliderValueToPos( double value, QRect & r );
	
    bool m_active;
	int m_x;
	int m_y;

    bool m_changingColor;
    bool m_changingSize;
    int m_lastX;
    int m_lastY;
	
	drwDrawingWidget * m_drawingWidget;
    drwLineTool * m_lineTool;

    // cache values initialized when widget is activated
    QRect m_boundingRect;
    QRect m_gradRect;
    QRect m_scaleRect;

    // Constants that define de geometry of the widget
    static const int m_width;
    static const int m_height;
    static const int m_spacing;
    static const int m_roundedCornerRadius;
    static const int m_scaleButtonRadius;
    static const int m_scaleSignRadius;
    static const int m_padding;
    static const int m_cursorWidth;
    static const int m_cursorHeight;
	
};


#endif
