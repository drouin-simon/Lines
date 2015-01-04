#ifndef __drwDrawingWidget_h_
#define __drwDrawingWidget_h_

#include <QGLWidget>
#include "Camera.h"
#include "macros.h"
#include "Scene.h"
#include "drwCommand.h"

class QPushButton;
class drwGLRenderer;
class drwWidgetObserver;
class drwDisplaySettings;
class drwLineToolViewportWidget;
class drwFpsCounter;
class PlaybackControler;
class drwDrawingWidgetInteractor;

class drwDrawingWidget : public QGLWidget
{
	Q_OBJECT
  
public:
		
	drwDrawingWidget( QWidget * parent=0 );
	~drwDrawingWidget();

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
    Camera * GetCamera();
    void ShowFullFrame( bool );
    Node * Pick( int x, int y );
    void WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld );

	drwCommand::s_ptr CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e );
	drwCommand::s_ptr CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e );
	
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
    drwDrawingWidgetInteractor * m_interactor;
    drwWidgetObserver * Observer;
    drwDisplaySettings	* DisplaySettings;
    drwLineToolViewportWidget * m_viewportWidget;
    drwFpsCounter * m_fpsCounter;
    drwCursor * m_cursor;
    bool m_showCursor;
}; 

#endif
