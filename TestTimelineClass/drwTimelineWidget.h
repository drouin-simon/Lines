#ifndef __drwTimelineWidget_h_
#define __drwTimelineWidget_h_

#include <QWidget>

class drwScene;

class drwTimelineWidget : public QWidget
{
	Q_OBJECT
  
public:
		
	drwTimelineWidget( QWidget * parent = 0 );
	void SetScene( drwScene * scene );
			
protected:
	
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);
	
	bool HitCell( int x, int y ) { return true; }
	
private:
	
	drwScene * m_scene;
	
	int m_layerHeight;
	int m_frameLength;
	
	int currentMouseX;
	int currentMouseY;
	int lastMouseX;
	int lastMouseY;
		
	bool mouseMoving;
	bool isScaling;
	bool isPaning;
	bool isMoving;
	
}; 

#endif
