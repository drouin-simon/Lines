#ifndef __drwLineToolViewportWidget_h_
#define __drwLineToolViewportWidget_h_

class drwDrawingWidget;
class drwLineTool;

class drwLineToolViewportWidget
{
	
public:
	
    drwLineToolViewportWidget( drwDrawingWidget * w, drwLineTool * lt );
	~drwLineToolViewportWidget();
	
    void Activate( int x, int y );
    void Deactivate();

	void MousePress( int x, int y );
	void MouseRelease( int x, int y );
    void MouseMove( int x, int y );
	void Draw();
	
protected:
	
    bool m_active;
	int m_x;
	int m_y;

    bool m_changingColor;
    bool m_changingSize;
    int m_lastX;
    int m_lastY;
	
	drwDrawingWidget * m_drawingWidget;
    drwLineTool * m_lineTool;

    // Constants that define de geometry of the widget
    static const int m_width;
    static const int m_height;
    static const int m_spacing;
    static const int m_cursorWidth;
    static const int m_cursorHeight;
	
};


#endif
