#ifndef __drwDrawingWidget_h_
#define __drwDrawingWidget_h_

#include <QOpenGLWidget>
#include "macros.h"
#include "Scene.h"
#include "drwCommand.h"
#include "drwDrawingSurface.h"
#include "Box2i.h"

class QPushButton;
class drwGLRenderer;
class drwWidgetObserver;
class drwLineToolViewportWidget;
class drwFpsCounter;
class PlaybackControler;
class drwCursor;

class drwDrawingWidget : public QOpenGLWidget, public drwDrawingSurface
{
	Q_OBJECT
  
public:
		
    drwDrawingWidget( QWidget * parent );
	~drwDrawingWidget();

    // Implement drwDrawingSurface interface
    void NeedRedraw();
    void NeedRedraw( int frame, Box2i & rect );
    void NeedRedrawOverlay();
    void NeedRedrawOverlay( int x, int y, int width, int height );

    void SetBackgroundColor( Vec4 & color );
    void SetCurrentScene( Scene * scene );
    SetMacro( Observer, drwWidgetObserver* );
    void SetControler( PlaybackControler * controler );
    GetMacro( Controler, PlaybackControler* );
    void SetViewportWidget( drwLineToolViewportWidget * w );
    void SetCursor( drwCursor * cursor );
    void SetCursorColor( QString colorName );
    void UpdatePosition( int x, int y );
    void SetMuteMouse( bool mute ) { m_muteMouse = mute; }
    bool IsMutingMouse() { return m_muteMouse; }
    void ToggleComputeFps();
    void ActivateViewportWidget( bool active );
    double PixelsPerUnit();

    void SimulateTabletEvent( drwMouseCommand::MouseCommandType type, double xWorld, double yWorld, double pressure );

    // Display settings
    int GetOnionSkinBefore() { return m_onionSkinFramesBefore; }
    void SetOnionSkinBefore( int );
    int GetOnionSkinAfter() { return m_onionSkinFramesAfter; }
    void SetOnionSkinAfter( int );
    bool GetInhibitOnionSkin() { return m_inhibitOnionSkin; }
    void SetInhibitOnionSkin( bool isOn );
	
public slots:

    void CurrentFrameChanged();
	void PlaybackStartStop( bool isStart );

signals:

    void FinishedPainting();
    void DisplaySettingsModified();
		
protected:
		
	virtual void initializeGL();
	virtual void resizeGL( int width, int height );
    virtual void paintEvent( QPaintEvent * event );
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void tabletEvent ( QTabletEvent * event );

    drwCommand::s_ptr CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e );
    drwCommand::s_ptr CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e );
	
	// Manage timer that generates updateGL in playback mode
	int m_timerId;
	
	void enterEvent( QEvent * );
	void leaveEvent( QEvent * );
	
	// stuff to do for all events
	bool event ( QEvent * event );

    void EnableVSync( bool enable );
    bool IsFrameDisplayed( int frame );
  
private:

    void PrintGLInfo();

    // Display settings
    int m_onionSkinFramesBefore;
    int m_onionSkinFramesAfter;
    bool m_inhibitOnionSkin;
	
    drwGLRenderer * m_renderer;
    PlaybackControler * Controler;
    drwWidgetObserver * Observer;
    drwLineToolViewportWidget * m_viewportWidget;
    drwFpsCounter * m_fpsCounter;
    drwCursor * m_cursor;
    bool m_showCursor;
    bool m_muteMouse;
    bool m_tabletHasControl;  // make sur not to generate both mouse and tablet events
    bool m_sceneModified;
    Box2i m_modifiedRect;
}; 

#endif
