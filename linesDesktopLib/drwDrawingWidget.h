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

    void SetBackgroundColor( Vec4 & color );
    void SetCurrentScene( Scene * scene );
    void SetToolbox( drwToolbox * t ) { m_toolbox = t; }
    void SetControler( PlaybackControler * controler );
    GetMacro( Controler, PlaybackControler* );
    void SetViewportWidget( drwLineToolViewportWidget * w );
    void SetMuteMouse( bool mute ) { m_muteMouse = mute; }
    bool IsMutingMouse() { return m_muteMouse; }
    void ActivateViewportWidget( bool active );
    double PixelsPerUnit();
    drwGLRenderer * GetRenderer() { return m_renderer; }

    void SimulateTabletEvent( drwMouseCommand::MouseCommandType type, double xWorld, double yWorld, double pressure );

    virtual void NotifyDisplaySettingsModified();
	
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
  
private:

    void PrintGLInfo();
	
    drwGLRenderer * m_renderer;
    PlaybackControler * Controler;
    drwToolbox * m_toolbox;
    drwLineToolViewportWidget * m_viewportWidget;
    bool m_muteMouse;
    bool m_tabletHasControl;  // make sur not to generate both mouse and tablet events
}; 

#endif
