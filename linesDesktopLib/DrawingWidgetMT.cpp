#include "DrawingWidgetMT.h"
#include "LinesCore.h"

#include <QTabletEvent>
#include <QOpenGLContext>
#include <QApplication>

RenderThread::RenderThread() : QThread(), m_drawingWidget(0)
{
}

RenderThread::~RenderThread()
{
}

void RenderThread::SetDrawingWidget( DrawingWidgetMT * c )
{
    m_drawingWidget = c;
}

void RenderThread::SetLinesCore( LinesCore * lc )
{
    m_lines = lc;
}

void RenderThread::run()
{
    // Do the rendering
    m_drawingWidget->makeCurrent();
    m_lines->Render();
    m_drawingWidget->doneCurrent();

    // return context to main thread
    m_drawingWidget->context()->moveToThread(QApplication::instance()->thread());

    // tell main thread to swap the buffer
    m_drawingWidget->update();
}

DrawingWidgetMT::DrawingWidgetMT( LinesCore * lc, QWidget * parent )
    : QOpenGLWidget( parent )
    , m_lines( lc )
    , m_timerId( -1 )
    , m_muteMouse( false )
    , m_tabletHasControl( false )
{
    // Create render thread instance
    m_renderThread = new RenderThread;
    m_renderThread->SetDrawingWidget( this );
    m_renderThread->SetLinesCore( m_lines );

    connect( this, SIGNAL(aboutToCompose()), this, SLOT(startCompositing()), Qt::DirectConnection );
    connect( this, SIGNAL(frameSwapped()), this, SLOT(finishCompositing()), Qt::DirectConnection );
    connect( this, SIGNAL(aboutToResize()), this, SLOT(StartResize()), Qt::DirectConnection );
    connect( this, SIGNAL(resized()), this, SLOT(FinishResize()), Qt::DirectConnection );
    connect( this, SIGNAL(PostRenderSignal()), this, SLOT(PostRenderSlot()), Qt::QueuedConnection );

    // Basic Qt widget properties
    setMouseTracking(true);
    setAutoFillBackground(false);
    setCursor( QCursor( Qt::BlankCursor ) );
    setUpdateBehavior( QOpenGLWidget::PartialUpdate );
}

DrawingWidgetMT::~DrawingWidgetMT()
{
    m_renderThread->wait();
    delete m_renderThread;
}

void DrawingWidgetMT::NotifyPlaybackStartStop( bool isStarting )
{
    // Start/stop generating idle events that are used to make sure we redraw during playback
    if( isStarting )
    {
        Q_ASSERT( m_timerId == -1 );
        m_timerId = startTimer(0);
    }
    else
    {
        Q_ASSERT( m_timerId != -1 );
        killTimer( m_timerId );
        m_timerId = -1;
    }
}

void DrawingWidgetMT::WaitRenderFinished()
{
    m_renderThread->wait();
}

void DrawingWidgetMT::Render()
{
    Q_ASSERT( !m_renderThread->isRunning() );
    context()->moveToThread( m_renderThread );
    m_renderThread->start();
}

void DrawingWidgetMT::TryRender()
{
    if( !m_renderThread->isRunning() /*&& m_lines->NeedsRender()*/ )
    {
        Render();
    }
}

void DrawingWidgetMT::PostRender()
{
    emit PostRenderSignal();
}

void DrawingWidgetMT::startCompositing()
{
    // need to wait for render thread to return context, in case it is running
    m_renderThread->wait();
}

void DrawingWidgetMT::finishCompositing()
{
    // buffer has just been swapped, we can start rendering again if there are pending lines to draw
    TryRender();
}

void DrawingWidgetMT::StartResize()
{
    // need to wait for render thread to return context, in case it is running
    m_renderThread->wait();
}

void DrawingWidgetMT::FinishResize()
{
    // This operation is padded by wait calls, i.e. renders
    // after a resize need to happen before this call returns
    // as Qt's event handling seem to require OpenGL context
    // to be owned by the GUI thread at some point after this
    // call and it is not notifying us. If the render thread
    // is launched at this point and we return, Qt crashes the
    // program because the main thread doesn't own the context
    // somewhere down the line.
    m_renderThread->wait();
    int ratio = this->devicePixelRatio();
    m_lines->SetRenderSize( ratio * width(), ratio * height() );
    context()->moveToThread( m_renderThread );
    m_renderThread->start();
    m_renderThread->wait();
}

void DrawingWidgetMT::PostRenderSlot()
{
    TryRender();
}

void DrawingWidgetMT::initializeGL()
{
    //cout << "Support threaded opengl: " << (this->context()->supportsThreadedOpenGL() ? "true" : "false") << endl;
}

void DrawingWidgetMT::tabletEvent( QTabletEvent * e )
{
    if( e->type() == QEvent::TabletPress )
    {
        m_tabletHasControl = true;
        MouseCommand( drwMouseCommand::Press, e );
    }
    else if( e->type() == QEvent::TabletRelease )
    {
        MouseCommand( drwMouseCommand::Release, e );
        m_tabletHasControl = false;
    }
    else if( e->type() == QEvent::TabletMove )
    {
        MouseCommand( drwMouseCommand::Move, e );
    }
    e->accept();
}

void DrawingWidgetMT::mousePressEvent( QMouseEvent * e )
{
    if( e->button() == Qt::LeftButton && !m_tabletHasControl  && !m_muteMouse )
    {
        MouseCommand( drwMouseCommand::Press, e );
    }
}

void DrawingWidgetMT::mouseReleaseEvent( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton && !m_tabletHasControl  && !m_muteMouse )
    {
        MouseCommand( drwMouseCommand::Release, e );
    }
}

void DrawingWidgetMT::mouseMoveEvent( QMouseEvent * e )
{
    if( !m_tabletHasControl && !m_muteMouse )
    {
        MouseCommand( drwMouseCommand::Move, e );
    }
}

void DrawingWidgetMT::enterEvent( QEvent * e )
{
    m_lines->SetShowCursor( true );
    TryRender();
}

void DrawingWidgetMT::leaveEvent( QEvent * e )
{
    m_lines->SetShowCursor( false );
    TryRender();
}

bool DrawingWidgetMT::event( QEvent * e )
{
    m_lines->Tick();
    TryRender();
    return QOpenGLWidget::event(e);
}

void DrawingWidgetMT::MouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e )
{
    int ratio = this->devicePixelRatio();
    double xWin = ratio * (double)e->x();
    double yWin = ratio * (double)e->y();
    m_lines->MouseEvent( commandType, xWin, yWin );
    TryRender();
}

void DrawingWidgetMT::MouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e )
{
    int ratio = this->devicePixelRatio();
    double deltaX = e->hiResGlobalX() - e->globalX();
    double xWin = ratio * ((double)e->x() + deltaX);
    double deltaY = e->hiResGlobalY() - e->globalY();
    double yWin = ratio * ((double)e->y() + deltaY);
    m_lines->MouseEvent( commandType, xWin, yWin, e->pressure(), e->xTilt(), e->yTilt(), e->rotation(), e->tangentialPressure() );
    TryRender();
}
