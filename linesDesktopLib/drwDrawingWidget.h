#ifndef __drwDrawingWidget_h_
#define __drwDrawingWidget_h_

#include <QOpenGLWidget>
#include "macros.h"
#include "Scene.h"
#include "drwCommand.h"
#include "drwDrawingSurface.h"

class QPushButton;
class drwGLRenderer;
class drwWidgetObserver;
class drwDisplaySettings;
class drwLineToolViewportWidget;
class drwFpsCounter;
class PlaybackControler;

class drwDrawingWidget : public QOpenGLWidget, public drwDrawingSurface
{
	Q_OBJECT
  
public:
		
    drwDrawingWidget( QWidget * parent, drwDisplaySettings * dispSettings );
	~drwDrawingWidget();

    void SetBackgroundColor( Vec4 & color );
    void SetCurrentScene( Scene * scene );
    SetMacro( Observer, drwWidgetObserver* );
    GetMacro( DisplaySettings, drwDisplaySettings* );
    void SetControler( PlaybackControler * controler );
    GetMacro( Controler, PlaybackControler* );
    void SetViewportWidget( drwLineToolViewportWidget * w );
    void SetCursor( drwCursor * cursor );
    void UpdatePosition( int x, int y );
    void ToggleComputeFps();
    void ActivateViewportWidget( bool active );
    double PixelsPerUnit();

	drwCommand::s_ptr CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e );
	drwCommand::s_ptr CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e );
    void SimulateTabletEvent( drwMouseCommand::MouseCommandType type, double xWorld, double yWorld, double pressure );
	
public slots:
	
	void RequestRedraw();
    void CurrentFrameChanged();
	void PlaybackStartStop( bool isStart );
	void DisplaySettingsModified();

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
	
	// Manage timer that generates updateGL in playback mode
	int m_timerId;
	
	void enterEvent( QEvent * );
	void leaveEvent( QEvent * );
	
	// Handling of Drag and Drop
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);
	
	// stuff to do for all events
	bool event ( QEvent * event );

    void EnableVSync( bool enable );
  
private:
	
    drwGLRenderer * m_renderer;
    PlaybackControler * Controler;
    drwWidgetObserver * Observer;
    drwDisplaySettings	* DisplaySettings;
    drwLineToolViewportWidget * m_viewportWidget;
    drwFpsCounter * m_fpsCounter;
    drwCursor * m_cursor;
    bool m_showCursor;
}; 

#endif
