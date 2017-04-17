#ifndef __drwDrawingWidget_h_
#define __drwDrawingWidget_h_

#include <QOpenGLWidget>
#include "drwCommand.h"
#include "drwDrawingSurface.h"

class LinesCore;
class drwGLRenderer;
class drwToolbox;
class drwLineToolViewportWidget;
class PlaybackControler;

class drwDrawingWidget : public QOpenGLWidget, public drwDrawingSurface
{
	Q_OBJECT
  
public:
		
    drwDrawingWidget( QWidget * parent );
	~drwDrawingWidget();

    // Implement drwDrawingSurface interface
    void NeedRedraw();
    void NotifyPlaybackStartStop( bool isStart );

    void SetViewportWidget( drwLineToolViewportWidget * w );
    void SetMuteMouse( bool mute ) { m_muteMouse = mute; }
    bool IsMutingMouse() { return m_muteMouse; }
    void ActivateViewportWidget( bool active );

signals:

    void FinishedPainting();
		
protected:
		
	virtual void initializeGL();
	virtual void resizeGL( int width, int height );
    virtual void paintEvent( QPaintEvent * event );
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void tabletEvent ( QTabletEvent * event );

    void MouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e );
    void MouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e );
	
	// Manage timer that generates updateGL in playback mode
	int m_timerId;
	
	void enterEvent( QEvent * );
	void leaveEvent( QEvent * );
	
	// stuff to do for all events
	bool event ( QEvent * event );

    void EnableVSync( bool enable );
  
private:

    void PrintGLInfo();
	
    LinesCore * m_lines;

    drwLineToolViewportWidget * m_viewportWidget;
    bool m_muteMouse;
    bool m_tabletHasControl;  // make sur not to generate both mouse and tablet events
}; 

#endif
