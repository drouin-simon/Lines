#include "drwDrawingWidget.h"
#include <iostream>
#include <QtGui>
#include <QtOpenGL>
#include "drwLineToolViewportWidget.h"
#include "LinesCore.h"

drwDrawingWidget::drwDrawingWidget( QWidget * parent )
: QOpenGLWidget( parent )
, m_viewportWidget(0)
, m_muteMouse(false)
, m_tabletHasControl(false)
{
    m_timerId = -1;
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

void drwDrawingWidget::NotifyPlaybackStartStop( bool isStarting )
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

void drwDrawingWidget::SetViewportWidget( drwLineToolViewportWidget * w )
{
    m_viewportWidget = w;
}

void drwDrawingWidget::initializeGL()
{
}

void drwDrawingWidget::resizeGL( int w, int h )
{
    int ratio = this->devicePixelRatio();
    m_lines->SetRenderSize( ratio*w, ratio*h );
    NeedRedraw();
}

void drwDrawingWidget::paintEvent( QPaintEvent * event )
{
	makeCurrent();

    m_lines->Render();

    // Draw the viewport widget if needed
    if( m_viewportWidget )
    {
        QPainter painter;
        painter.begin( this );
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);
        m_viewportWidget->Draw( painter );
        painter.end();
    }

    emit FinishedPainting();
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
	// Try the widget
	bool widgetSwallows = m_viewportWidget->MousePress( e->x(), e->y() );
	if( widgetSwallows )
		return;
	
	// now send the event to observer ( drawing tools)
    if( e->button() == Qt::LeftButton && !m_tabletHasControl  && !m_muteMouse )
	{
        MouseCommand( drwMouseCommand::Press, e );
	}
}

void drwDrawingWidget::mouseReleaseEvent( QMouseEvent * e )
{
	// Try the widget
	bool widgetSwallows = m_viewportWidget->MouseRelease( e->x(), e->y() );
	if( widgetSwallows )
		return;
	
    if ( e->button() == Qt::LeftButton && !m_tabletHasControl  && !m_muteMouse )
    {
        MouseCommand( drwMouseCommand::Release, e );
    }
}


void drwDrawingWidget::mouseMoveEvent( QMouseEvent * e )
{   
	// Try the widget
	bool widgetSwallows = m_viewportWidget->MouseMove( e->x(), e->y() );
	if( widgetSwallows )
		return;
	
    if( !m_tabletHasControl && !m_muteMouse )
    {
        MouseCommand( drwMouseCommand::Move, e );
    }
}

void drwDrawingWidget::tabletEvent ( QTabletEvent * e )
{
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
	
    if( e->type() == QEvent::TabletPress )
    {
        m_tabletHasControl = true;
        MouseCommand( drwMouseCommand::Press, e );
        e->accept();
    }
    else if( e->type() == QEvent::TabletRelease )
    {
        MouseCommand( drwMouseCommand::Release, e );
        e->accept();
        m_tabletHasControl = false;
    }
    else if( e->type() == QEvent::TabletMove )
    {
        MouseCommand( drwMouseCommand::Move, e );
        e->accept();
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

void drwDrawingWidget::enterEvent( QEvent * e )
{
    m_lines->SetShowCursor( true );
}

void drwDrawingWidget::leaveEvent( QEvent * e )
{
    if( m_viewportWidget )
        m_viewportWidget->Deactivate();
    m_lines->SetShowCursor( false );
    NeedRedraw();
}

bool drwDrawingWidget::event( QEvent * e )
{
	// Give a chance to the controler to change frame. A frame change triggers an updateGL
    m_lines->Tick();
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

void drwDrawingWidget::MouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e )
{
    int ratio = this->devicePixelRatio();
    double xWin = ratio * (double)e->x();
    double yWin = ratio * (double)e->y();
    m_lines->MouseEvent( commandType, xWin, yWin );
}

void drwDrawingWidget::MouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e )
{
    int ratio = this->devicePixelRatio();
    double deltaX = e->hiResGlobalX() - e->globalX();
    double xWin = ratio * ((double)e->x() + deltaX);
    double deltaY = e->hiResGlobalY() - e->globalY();
    double yWin = ratio * ((double)e->y() + deltaY);
    m_lines->MouseEvent( commandType, xWin, yWin, e->pressure(), e->xTilt(), e->yTilt(), e->rotation(), e->tangentialPressure() );
}
