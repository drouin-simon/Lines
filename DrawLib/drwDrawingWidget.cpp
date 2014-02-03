#include "drwDrawingWidget.h"
#include <QtGui>
#include <QtOpenGL>

#include "Scene.h"
#include "drwWidgetObserver.h"
#include "PlaybackControler.h"
#include "drwDisplaySettings.h"
#include "drwDrawingContext.h"
#include "drwDrawableTexture.h"
#include "drwDrawingWidgetInteractor.h"
#include "drwLineToolViewportWidget.h"
#include "drwCursor.h"
#include "drwFpsCounter.h"
#include "drwGLRenderer.h"
#include "SoundGenerator.h"

drwDrawingWidget::drwDrawingWidget( QWidget * parent ) 
: QGLWidget( QGLFormat(QGL::SampleBuffers), parent ) 
, Observer(0)
, m_viewportWidget(0)
, m_cursor(0)
, m_showCursor(false)
{
    m_timerId = -1;
	m_interactor = new drwDrawingWidgetInteractor( this );
    this->Controler = 0;
    m_fpsCounter = 0;

	DisplaySettings = new drwDisplaySettings;
	connect( DisplaySettings, SIGNAL(ModifiedSignal()), this, SLOT(DisplaySettingsModified()) );

    m_renderer = new drwGLRenderer;
    connect( m_renderer, SIGNAL(NeedRenderSignal()), this, SLOT(RequestRedraw()) );

    m_soundGenerator = new SoundGenerator;

    setAcceptDrops(true);
	setMouseTracking(true);
	setCursor( Qt::BlankCursor );
	setFocusPolicy(Qt::StrongFocus);
	setAutoFillBackground(false);
}

drwDrawingWidget::~drwDrawingWidget()
{
	delete m_interactor;
	delete DisplaySettings;
    delete m_soundGenerator;
}

drwCommand::s_ptr drwDrawingWidget::CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e )
{
	double xWin = (double)e->x();
	double yWin = (double)e->y();
	double xWorld = 0.0;
	double yWorld = 0.0;
    m_renderer->WindowToWorld( xWin, yWin, xWorld, yWorld );
    drwCommand::s_ptr command( new drwMouseCommand( commandType, xWorld, yWorld, 0.0, xWin, yWin, 0, 0, 1.0, 0.0, 0.0 ) );
	return command;
}

drwCommand::s_ptr drwDrawingWidget::CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e )
{
	double deltaX = e->hiResGlobalX() - e->globalX();
	double xWin = (double)e->x() + deltaX;
	double deltaY = e->hiResGlobalY() - e->globalY();
	double yWin = (double)e->y() + deltaY;
	double xWorld = 0.0;
	double yWorld = 0.0;
    m_renderer->WindowToWorld( xWin, yWin, xWorld, yWorld );
    drwCommand::s_ptr command( new drwMouseCommand( commandType, xWorld, yWorld, 0.0, xWin, yWin, e->xTilt(), e->yTilt(), e->pressure(), e->rotation(), e->tangentialPressure() ) );
	return command;
}

void drwDrawingWidget::SetCurrentScene( Scene * scene )
{
    m_renderer->SetCurrentScene( scene );
}

void drwDrawingWidget::SetControler( PlaybackControler * controler )
{
    Controler = controler;
    connect(Controler, SIGNAL(FrameChanged(int)), SLOT(CurrentFrameChanged()) );
    connect(Controler, SIGNAL(StartStop(bool)), SLOT(PlaybackStartStop(bool)) );
}

void drwDrawingWidget::SetViewportWidget( drwLineToolViewportWidget * w )
{
    m_viewportWidget = w;
}

void drwDrawingWidget::SetCursor( drwCursor * cursor )
{
    m_cursor = cursor;
    connect( m_cursor, SIGNAL(Modified()), this, SLOT(RequestRedraw()) );
}

void drwDrawingWidget::ToggleComputeFps()
{
    if( m_fpsCounter )
    {
        delete m_fpsCounter;
        m_fpsCounter = 0;
    }
    else
    {
        m_fpsCounter = new drwFpsCounter;
        m_fpsCounter->Start();
    }
}

void drwDrawingWidget::PlaySound()
{
    makeCurrent();

    m_renderer->RenderToTexture( Controler->GetCurrentFrame() );
    m_soundGenerator->GenerateFramesForImage( m_renderer->GetRenderTexture() );
    m_soundGenerator->Play();
}

void drwDrawingWidget::RequestRedraw()
{
    update();
}

void drwDrawingWidget::CurrentFrameChanged()
{
	update();
	Observer->SetCurrentFrame( Controler->GetCurrentFrame() );
}

void drwDrawingWidget::PlaybackStartStop( bool isStarting )
{
	// Start/stop generating idle events that are used to make sure we redraw during playback
	if( isStarting )
	{
        // render current frame to texture, generate sound from frame
        m_renderer->RenderToTexture( Controler->GetCurrentFrame() );
        //m_soundGenerator->GenerateFramesForImage( m_renderer->GetRenderTexture() );

        // start timer
		if( m_timerId == -1 )
			m_timerId = startTimer(0);
        EnableVSync( true );

        // start playing sound
        //m_soundGenerator->Play();
	}
	else
	{
		if( m_timerId != -1 )
		{
			killTimer( m_timerId );
			m_timerId = -1;
		}
        EnableVSync( false );
        //m_soundGenerator->Stop();
	}
	
	DisplaySettings->SetInhibitOnionSkin( isStarting );
}

void drwDrawingWidget::DisplaySettingsModified()
{
	update();
}

void drwDrawingWidget::initializeGL()
{
}

void drwDrawingWidget::resizeGL( int w, int h )
{
    m_renderer->SetRenderSize( w, h );
}

void drwDrawingWidget::paintEvent( QPaintEvent * event )
{
	makeCurrent();
	
    if( Controler->IsPlaying() )
    {
        // paste previously rendered image to screen
        m_renderer->RenderTextureToScreen();
        
        // Render next image
        m_renderer->RenderToTexture( Controler->GetNextFrame() );

        // on start playing: generate soundtrack for current frame and start playing sound
        // here: generate soundtrack for next frame
        //
    }
    else if( DisplaySettings->GetShowAllFrames() )
    {
        m_renderer->RenderAllFrames( Controler->GetCurrentFrame() );
    }
    else
    {
        int onionSkinBefore = DisplaySettings->GetOnionSkinBefore();
        int onionSkinAfter = DisplaySettings->GetOnionSkinAfter();
        if( DisplaySettings->GetInhibitOnionSkin() )
        {
            onionSkinBefore = 0;
            onionSkinAfter = 0;
        }
        m_renderer->Render( Controler->GetCurrentFrame(), onionSkinBefore, onionSkinAfter );
    }

    if( DisplaySettings->GetShowCameraFrame() )
        m_renderer->RenderCameraFrame();

    QPainter painter;
    painter.begin( this );
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Draw the viewport widget if needed
    if( m_viewportWidget )
    {
        m_viewportWidget->Draw( painter );
    }

    // Draw fps counter if needed
    if( m_fpsCounter && m_fpsCounter->IsRunning() )
    {
        m_fpsCounter->Tick();
        QString text = QString("fps: %1").arg( m_fpsCounter->GetFps() );
        painter.drawText( 5, this->height() - 5, text );
    }

    // draw the cursor if needed
    if( m_cursor && m_showCursor )
    {
        m_cursor->Draw( painter );
    }

    painter.end();

    emit FinishedPainting();
}

void drwDrawingWidget::UpdatePosition( int x, int y )
{
    m_cursor->SetPosition( x, y );
}

#ifdef Q_WS_MAC
#import <OpenGL/OpenGL.h>
#endif

void drwDrawingWidget::EnableVSync( bool enable )
{
#ifdef Q_WS_MAC
    int swap_interval = 1;
    if( !enable )
        swap_interval = 0;
    CGLContextObj cgl_context = CGLGetCurrentContext();
    CGLSetParameter( cgl_context, kCGLCPSwapInterval, &swap_interval );
#endif
}

void drwDrawingWidget::mousePressEvent( QMouseEvent * e )
{
    // update the cursor position and all that stuff
    UpdatePosition( e->x(), e->y() );

	// Try the widget
	bool widgetSwallows = m_viewportWidget->MousePress( e->x(), e->y() );
	if( widgetSwallows )
		return;
	
	// try the interactor
	bool interactorSwallows = m_interactor->mousePressEvent( e );
	if( interactorSwallows )
		return;
	
	// now send the event to observer ( drawing tools)
	if( e->button() == Qt::LeftButton && Observer )
	{
		Observer->MousePressEvent( this, e );
	}
}


void drwDrawingWidget::mouseReleaseEvent( QMouseEvent * e )
{
    // update the cursor position and all that stuff
    UpdatePosition( e->x(), e->y() );

	// Try the widget
	bool widgetSwallows = m_viewportWidget->MouseRelease( e->x(), e->y() );
	if( widgetSwallows )
		return;
	
	// try the interactor
	bool interactorSwallows = m_interactor->mouseReleaseEvent( e );
	if( interactorSwallows )
		return;
	
	if ( e->button() == Qt::LeftButton && Observer )
		Observer->MouseReleaseEvent( this, e );
}


void drwDrawingWidget::mouseMoveEvent( QMouseEvent * e )
{
    // update the cursor position and all that stuff
    UpdatePosition( e->x(), e->y() );

	// Try the widget
	bool widgetSwallows = m_viewportWidget->MouseMove( e->x(), e->y() );
	if( widgetSwallows )
		return;
	
	// try the interactor first
	bool interactorSwallows = m_interactor->mouseMoveEvent( e );
	if( interactorSwallows )
		return;
	
	if ( Observer )
		Observer->MouseMoveEvent( this, e );
}

void drwDrawingWidget::tabletEvent ( QTabletEvent * e )
{
    // update the cursor position and all that stuff
    UpdatePosition( e->x(), e->y() );

    // Try the widget
    bool widgetSwallows = false;
    if( e->type() == QEvent::TabletPress )
        widgetSwallows = m_viewportWidget->MousePress( e->x(), e->y() );
    else if( e->type() == QEvent::TabletRelease )
        widgetSwallows = m_viewportWidget->MouseRelease( e->x(), e->y() );
    else if( e->type() == QEvent::TabletMove )
        widgetSwallows = m_viewportWidget->MouseMove( e->x(), e->y() );
    if( widgetSwallows )
        return;

    // try the interactor
	bool interactorSwallows = m_interactor->tabletEvent( e );
	if( interactorSwallows )
		return;
	
	if( Observer )
		Observer->TabletEvent( this, e );
}

void drwDrawingWidget::ActivateViewportWidget( bool active )
{
    if( active )
    {
        QPoint p = this->mapFromGlobal(QCursor::pos());
        m_viewportWidget->Activate( p.x(), p.y() );
        update();
    }
    else
    {
        m_viewportWidget->Deactivate();
        update();
    }
}

Camera * drwDrawingWidget::GetCamera()
{
    return m_renderer->GetCamera();
}

void drwDrawingWidget::ShowFullFrame( bool show )
{
    m_renderer->ShowFullFrame( show );
    update();
}

Node * drwDrawingWidget::Pick( int x, int y )
{
    return m_renderer->Pick( Controler->GetCurrentFrame(), x, y );
}

void drwDrawingWidget::WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld )
{
    m_renderer->WindowToWorld( xWin, yWin, xWorld, yWorld );
}

void drwDrawingWidget::enterEvent( QEvent * e )
{
    m_showCursor = true;
}

void drwDrawingWidget::leaveEvent( QEvent * e )
{
    m_showCursor = false;
    if( m_viewportWidget )
        m_viewportWidget->Deactivate();
	update();
}

void drwDrawingWidget::dragEnterEvent(QDragEnterEvent *event)
 {
     event->acceptProposedAction();
 }

 void drwDrawingWidget::dragMoveEvent(QDragMoveEvent *event)
 {
     event->acceptProposedAction();
 }

 void drwDrawingWidget::dropEvent(QDropEvent * event)
 {
	 Observer->DropEvent( this, event );
     event->acceptProposedAction();
 }

 void drwDrawingWidget::dragLeaveEvent(QDragLeaveEvent *event)
 {
     event->accept();
 }

bool drwDrawingWidget::event( QEvent * e )
{
	// Give a chance to the controler to change frame. A frame change triggers an updateGL
	bool controlerUpdated = false;
	if( Controler )
	{
		controlerUpdated = Controler->Tick();
    }
	
	return QGLWidget::event(e);
}
