#ifndef __drwLineToolViewportWidget_h_
#define __drwLineToolViewportWidget_h_

class drwDrawingWidget;

class drwLineToolViewportWidget
{
	
public:
	
	drwLineToolViewportWidget( drwDrawingWidget * w, int x, int y );
	~drwLineToolViewportWidget();
	
	void MousePress( int x, int y );
	void MouseRelease( int x, int y );
	void Draw();
	
protected:
	
	int m_x;
	int m_y;
	
	drwDrawingWidget * m_drawingWidget;
	
};


#endif