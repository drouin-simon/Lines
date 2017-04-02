#include "drwDrawingWidget.h"
#include <iostream>
#include <QtGui>
#include <QtOpenGL>
#include "Vec4.h"
#include "Scene.h"
#include "PlaybackControler.h"
#include "drwDrawingContext.h"
#include "drwDrawableTexture.h"
#include "drwLineToolViewportWidget.h"
#include "drwCursor.h"
#include "drwGLRenderer.h"
#include "drwToolbox.h"

drwDrawingWidget::drwDrawingWidget( QWidget * parent )
: QOpenGLWidget( parent )
, m_toolbox(0)
, m_viewportWidget(0)
, m_cursor(0)
, m_showCursor(false)
, m_muteMouse(false)
, m_tabletHasControl(false)
{
    m_timerId = -1;
    this->Controler = 0;

    m_renderer = new drwGLRenderer;
    m_renderer->SetDrawingSurface( this );

    //PrintGLInfo();

    setAcceptDrops(true);
	setMouseTracking(true);
	setAutoFillBackground(false);
    setCursor( QCursor( Qt::BlankCursor ) );
    setUpdateBehavior( QOpenGLWidget::PartialUpdate );  // allows to redraw only part of the window
    EnableVSync( false );
}

drwDrawingWidget::~drwDrawingWidget()
{
}

void drwDrawingWidget::NeedRedraw()
{
    update();
}

void drwDrawingWidget::NotifyDisplaySettingsModified()
{
    emit DisplaySettingsModified();
}

void drwDrawingWidget::SimulateTabletEvent( drwMouseCommand::MouseCommandType type, double xWorld, double yWorld, double pressure )
{
    int xWin, yWin;
    m_renderer->WorldToGLWindow( xWorld, yWorld, xWin, yWin );
    drwCommand::s_ptr command( new drwMouseCommand( type, xWorld, yWorld, 0.0, xWin, yWin, 0, 0, pressure, 0, 0 ) );
    m_toolbox->ExecuteCommand( command );
}

void drwDrawingWidget::SetBackgroundColor( Vec4 & color )
{
    m_renderer->SetClearColor( color[0], color[1], color[2], color[3] );
}

void drwDrawingWidget::SetCurrentScene( Scene * scene )
{
    m_renderer->SetCurrentScene( scene );
}

void drwDrawingWidget::SetControler( PlaybackControler * controler )
{
    Controler = controler;
    connect(Controler, SIGNAL(FrameChanged()), SLOT(CurrentFrameChanged()) );
    connect(Controler, SIGNAL(StartStop(bool)), SLOT(PlaybackStartStop(bool)) );
}

void drwDrawingWidget::SetViewportWidget( drwLineToolViewportWidget * w )
{
    m_viewportWidget = w;
}

void drwDrawingWidget::SetCursor( drwCursor * cursor )
{
    m_cursor = cursor;
}

void drwDrawingWidget::SetCursorColor( QString colorName )
{
    m_cursor->SetColor( colorName );
}

void drwDrawingWidget::CurrentFrameChanged()
{
    NeedRedraw();
}

void drwDrawingWidget::PlaybackStartStop( bool isStarting )
{
	// Start/stop generating idle events that are used to make sure we redraw during playback
	if( isStarting )
	{
        // start timer
		if( m_timerId == -1 )
			m_timerId = startTimer(0);
        EnableVSync( true );
	}
	else
	{
		if( m_timerId != -1 )
		{
			killTimer( m_timerId );
			m_timerId = -1;
		}
        EnableVSync( false );
	}
}

void drwDrawingWidget::initializeGL()
{
}

void drwDrawingWidget::resizeGL( int w, int h )
{
    int ratio = this->devicePixelRatio();
    m_renderer->SetRenderSize( ratio*w, ratio*h );
    NeedRedraw();
}

void drwDrawingWidget::paintEvent( QPaintEvent * event )
{
	makeCurrent();

    m_renderer->Render();

    QPainter painter;
    painter.begin( this );
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Draw the viewport widget if needed
    if( m_viewportWidget )
    {
        m_viewportWidget->Draw( painter );
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
	
	// now send the event to observer ( drawing tools)
    if( e->button() == Qt::LeftButton && m_toolbox && !m_tabletHasControl  && !m_muteMouse )
	{
        drwCommand::s_ptr command = CreateMouseCommand( drwMouseCommand::Press, e );
        m_toolbox->ExecuteCommand( command );
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
	
    if ( e->button() == Qt::LeftButton && m_toolbox && !m_tabletHasControl  && !m_muteMouse )
    {
        drwCommand::s_ptr command = CreateMouseCommand( drwMouseCommand::Release, e );
        m_toolbox->ExecuteCommand( command );
    }
}


void drwDrawingWidget::mouseMoveEvent( QMouseEvent * e )
{   
    // update the cursor position and all that stuff
    UpdatePosition( e->x(), e->y() );

	// Try the widget
	bool widgetSwallows = m_viewportWidget->MouseMove( e->x(), e->y() );
	if( widgetSwallows )
		return;
	
    if ( m_toolbox && !m_tabletHasControl && !m_muteMouse )
    {
        drwCommand::s_ptr command = CreateMouseCommand( drwMouseCommand::Move, e );
        m_toolbox->ExecuteCommand( command );
    }
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
    {
        e->accept();
        return;
    }
	
    if( m_toolbox )
    {
        if( e->type() == QEvent::TabletPress )
        {
            m_tabletHasControl = true;
            drwCommand::s_ptr command = CreateMouseCommand( drwMouseCommand::Press, e );
            m_toolbox->ExecuteCommand( command );
            e->accept();
        }
        else if( e->type() == QEvent::TabletRelease )
        {
            drwCommand::s_ptr command = CreateMouseCommand( drwMouseCommand::Release, e );
            m_toolbox->ExecuteCommand( command );
            e->accept();
            m_tabletHasControl = false;
        }
        else if( e->type() == QEvent::TabletMove )
        {
            drwCommand::s_ptr command = CreateMouseCommand( drwMouseCommand::Move, e );
            m_toolbox->ExecuteCommand( command );
            e->accept();
        }
    }
}

void drwDrawingWidget::ActivateViewportWidget( bool active )
{
    if( active )
    {
        QPoint p = this->mapFromGlobal(QCursor::pos());
        m_viewportWidget->Activate( p.x(), p.y() );
    }
    else
    {
        m_viewportWidget->Deactivate();
    }
    NeedRedraw();
}

double drwDrawingWidget::PixelsPerUnit()
{
    return m_renderer->PixelsPerUnit();
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
    NeedRedraw();
}

bool drwDrawingWidget::event( QEvent * e )
{
	// Give a chance to the controler to change frame. A frame change triggers an updateGL
	bool controlerUpdated = false;
	if( Controler )
	{
		controlerUpdated = Controler->Tick();
    }
	
    return QOpenGLWidget::event(e);
}

void drwDrawingWidget::PrintGLInfo()
{
    QSurfaceFormat f = this->format();

    // OpenGL version
    std::cout << "OpenGL version: " << f.majorVersion() << "." << f.minorVersion() << std::endl;

    // OpenGL profile
    std::cout << "OpenGL profile: ";
    if( f.profile() == QSurfaceFormat::NoProfile )
        std::cout << "NoProfile" << std::endl;
    else if( f.profile() == QSurfaceFormat::CoreProfile )
        std::cout << "CoreProfile" << std::endl;
    else if( f.profile() == QSurfaceFormat::CompatibilityProfile )
        std::cout << "CompatibilityProfile" << std::endl;

    // Swap behavior
    std::cout << "Swap behavior: ";
    if( f.swapBehavior() == QSurfaceFormat::DefaultSwapBehavior )
        std::cout << "DefaultSwapBehavior" << std::endl;
    else if( f.swapBehavior() == QSurfaceFormat::SingleBuffer )
        std::cout << "SingleBuffer" << std::endl;
    else if( f.swapBehavior() == QSurfaceFormat::DoubleBuffer )
        std::cout << "DoubleBuffer" << std::endl;
    else if( f.swapBehavior() == QSurfaceFormat::TripleBuffer )
        std::cout << "TripleBuffer" << std::endl;

    // Update behavior
    std::cout << "Update behavior: ";
    if( this->updateBehavior() == QOpenGLWidget::NoPartialUpdate )
        std::cout << "NoPartialUpdate" << std::endl;
    else if( this->updateBehavior() == QOpenGLWidget::PartialUpdate )
        std::cout << "PartialUpdate" << std::endl;
}

drwCommand::s_ptr drwDrawingWidget::CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e )
{
    int ratio = this->devicePixelRatio();
    double xWin = ratio * (double)e->x();
    double yWin = ratio * (double)e->y();
    double xWorld = 0.0;
    double yWorld = 0.0;
    m_renderer->WindowToWorld( xWin, yWin, xWorld, yWorld );
    drwCommand::s_ptr command( new drwMouseCommand( commandType, xWorld, yWorld, 0.0, xWin, yWin, 0, 0, 1.0, 0.0, 0.0 ) );
    return command;
}

drwCommand::s_ptr drwDrawingWidget::CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e )
{
    int ratio = this->devicePixelRatio();
    double deltaX = e->hiResGlobalX() - e->globalX();
    double xWin = ratio * ((double)e->x() + deltaX);
    double deltaY = e->hiResGlobalY() - e->globalY();
    double yWin = ratio * ((double)e->y() + deltaY);
    double xWorld = 0.0;
    double yWorld = 0.0;
    m_renderer->WindowToWorld( xWin, yWin, xWorld, yWorld );
    drwCommand::s_ptr command( new drwMouseCommand( commandType, xWorld, yWorld, 0.0, xWin, yWin, e->xTilt(), e->yTilt(), e->pressure(), e->rotation(), e->tangentialPressure() ) );
    return command;
}
