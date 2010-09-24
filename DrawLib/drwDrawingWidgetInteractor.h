#ifndef _drwDrawingWidgetInteractor_h_
#define _drwDrawingWidgetInteractor_h_

#include <QMouseEvent>
#include <QTabletEvent>

class drwDrawingWidget;

class drwDrawingWidgetInteractor
{
	
public:
	
	drwDrawingWidgetInteractor( drwDrawingWidget * widget );
	~drwDrawingWidgetInteractor();
	
	// all these functions return true if they swallow events
	bool mousePressEvent( QMouseEvent* );
	bool mouseReleaseEvent( QMouseEvent* );
	bool mouseMoveEvent( QMouseEvent* );
	bool tabletEvent ( QTabletEvent* );
	
protected:
	
	bool Press( int x, int y, Qt::KeyboardModifiers modifiers );
	bool Release();
	bool Move( int x, int y );
	
	int currentMouseX;
	int currentMouseY;
	int lastMouseX;
	int lastMouseY;
	
	bool mouseMoving;
	bool isScaling;
	bool isPaning;
	
	drwDrawingWidget * m_widget;
	
};

#endif