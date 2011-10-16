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
#include "drwDrawingEngine.h"

drwDrawingWidget::drwDrawingWidget( QWidget * parent ) 
: QGLWidget( QGLFormat(QGL::SampleBuffers), parent )
, m_timerId(-1)
, Observer(0)
, CurrentScene(0)
, Controler(0)
, m_fpsCounter(0)
, DisplaySettings(0)
, m_viewportWidget(0)
, m_cursor(0)
, m_drawingEngine(0)
, m_showCursor(false)
, m_showFullFrame(true)
, m_framePadding(0.0)
{
	m_interactor = new drwDrawingWidgetInteractor( this );
	DisplaySettings = new drwDisplaySettings;
	m_workTexture = new drwDrawableTexture;
	connect( DisplaySettings, SIGNAL(ModifiedSignal()), this, SLOT(DisplaySettingsModified()) );
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
	delete m_workTexture;
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

void drwDrawingWidget::StartDrawingEngine()
{
    if( m_timerId == -1 )
        m_timerId = startTimer(0);
}

void drwDrawingWidget::StopDrawingEngine()
{
    if( m_timerId != -1 )
        killTimer( m_timerId );
    m_timerId = -1;
}

drwCommand::s_ptr drwDrawingWidget::CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e )
{
	double xWin = (double)e->x();
	double yWin = (double)e->y();
	double xWorld = 0.0;
	double yWorld = 0.0;
	theCamera.WindowToWorld( xWin, yWin, xWorld, yWorld );
	drwCommand::s_ptr command( new drwMouseCommand( commandType, xWorld, yWorld, 0.0, 0, 0, 1.0, 0.0, 0.0 ) );
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
	theCamera.WindowToWorld( xWin, yWin, xWorld, yWorld );
	drwCommand::s_ptr command( new drwMouseCommand( commandType, xWorld, yWorld, 0.0, e->xTilt(), e->yTilt(), e->pressure(), e->rotation(), e->tangentialPressure() ) );
	return command;
}


void drwDrawingWidget::WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld )
{
	theCamera.WindowToWorld( xWin, yWin, xWorld, yWorld );
}

void drwDrawingWidget::WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin )
{
    theCamera.WorldToGLWindow( xworld, yworld, xwin, ywin );
}

double drwDrawingWidget::PixelsPerUnit()
{
    return theCamera.PixelsPerUnit();
}

void drwDrawingWidget::ShowFullFrame( bool show )
{
    m_showFullFrame = show;
    if( m_showFullFrame )
        theCamera.FitRectInside( CurrentScene->GetFrameWidth(), CurrentScene->GetFrameHeight(), m_framePadding );
    update();
}

#define PICK_BUF_SIZE 512
GLuint selectBuf[PICK_BUF_SIZE];

Node * drwDrawingWidget::Pick( int x, int y )
{
	// Enter selection mode and initialize hit buffer
	glSelectBuffer(PICK_BUF_SIZE,selectBuf);
	glRenderMode(GL_SELECT);

	// Place the camera for picking (reduced viewport)
	theCamera.PlaceCameraForPicking( x, y );

	glInitNames();

	// Render
	glLoadIdentity();
	drwDrawingContext context(this);
	context.m_isPicking = true;
	CurrentScene->DrawFrame( Controler->GetCurrentFrame(), context );

	// returning to normal rendering mode
	glFlush();
	int nbHits = glRenderMode(GL_RENDER);

	// Get the Id of the last hit (last one is drawn last and thus on top )
	GLuint * it = selectBuf;
	GLuint id = 0xffffffff;
	for( int i = 0; i < nbHits; ++i)
	{
		GLuint nbNames = *it;
		++it; ++it; ++it;  // jump over min and max depth info
		for( GLuint j = 0; j < nbNames; ++j )
		{
			if( i == nbHits - 1 && j == nbNames - 1 )
				id = *it;
			++it;
		}
	}

	if( id != 0xffffffff )
	{
		return CurrentScene->GetNodeById( Controler->GetCurrentFrame(), id );
	}

	return 0;
}

void drwDrawingWidget::SetCurrentScene( Scene * cur )
{
	CurrentScene = cur;
	connect( CurrentScene, SIGNAL( Modified() ), SLOT( RequestRedraw() ), Qt::DirectConnection );
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
		if( m_timerId == -1 )
			m_timerId = startTimer(0);
	}
	else
	{
		if( m_timerId != -1 )
		{
			killTimer( m_timerId );
			m_timerId = -1;
		}
	}
	
	DisplaySettings->SetInhibitOnionSkin( isStarting );
}

void drwDrawingWidget::DisplaySettingsModified()
{
	update();
}


//====================================================================
// Setup all Gl states that are common to all types of primitive
// TODO: Create a class that will contain those settings?
//====================================================================
void drwDrawingWidget::initializeGL()
{
	// Enable texture mapping for sprites
	//glEnable( GL_TEXTURE_2D );
	glEnable( GL_TEXTURE_RECTANGLE_ARB );
	glDisable( GL_DEPTH_TEST );
	
	// Enable antialiasing of lines
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_BLEND );     
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 

	// Enable vertex arrays: needed by geometry classes
	glEnableClientState( GL_VERTEX_ARRAY );
	
	// Default background color
	glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );

    // initialize modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Initialize the working texture (used to draw widelines, among others)
	m_workTexture->Init( 1, 1 );

    EnableVSync(true);
}


void drwDrawingWidget::resizeGL( int w, int h )
{
	theCamera.SetViewportSize( w, h );
    if( m_showFullFrame )
        theCamera.FitRectInside( CurrentScene->GetFrameWidth(), CurrentScene->GetFrameHeight(), m_framePadding );
	m_workTexture->Resize( w, h );
}

void drwDrawingWidget::paintEvent( QPaintEvent * /*event*/ )
{
	makeCurrent();
	
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// needed by line drawing code. simtodo : should not be here.
	glEnable( GL_TEXTURE_RECTANGLE_ARB );
	glEnable( GL_BLEND );     
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
	
	// Place virtual camera
	theCamera.PlaceCamera();
	glLoadIdentity();
	
	if( CurrentScene )
	{
		// Draw the Scene
		if( DisplaySettings->GetShowAllFrames() )
		{
			//DrawAllFramesHue();
			DrawAllFramesRedGreen();
		}
		else if( !DisplaySettings->GetInhibitOnionSkin() )
		{
			drwDrawingContext c(this);

            if( DisplaySettings->GetOnionSkinBefore() > 0 )
            {
                Vec4 onionSkinColor( 1.0, 0.90, 0.90, 1.0 );
                int nbOnionSkinBefore = DisplaySettings->GetOnionSkinBefore();
                for( int i = 0; i < nbOnionSkinBefore; ++i )
                {
                    int frameToDraw = Controler->GetCurrentFrame() - nbOnionSkinBefore + i;
                    if( frameToDraw >= 0 )
                    {
                        double halfDiff = (double)( Controler->GetCurrentFrame() - frameToDraw ) * .2;
                        double onionSkinOpacity = 1 / exp( halfDiff );
                        c.m_colorMultiplier = onionSkinOpacity * onionSkinColor;
                        c.m_colorMultiplier[3] = 1.0;
                        CurrentScene->DrawFrame( frameToDraw, c );
                    }
                }
            }

            if( DisplaySettings->GetOnionSkinAfter() )
            {
                Vec4 onionSkinColor( .90, .90, 1.0, 1.0 );
                int nbOnionSkinAfter = DisplaySettings->GetOnionSkinAfter();
                for( int i = 0; i < nbOnionSkinAfter; ++i )
                {
                    int frameToDraw = Controler->GetCurrentFrame() + nbOnionSkinAfter - i;
                    if( frameToDraw < CurrentScene->GetNumberOfFrames() - 1 )
                    {
                        double halfDiff = (double)( frameToDraw - Controler->GetCurrentFrame() ) * .2;
                        double onionSkinOpacity = 1 / exp( halfDiff );
                        c.m_colorMultiplier = onionSkinOpacity * onionSkinColor;
                        c.m_colorMultiplier[3] = 1.0;
                        CurrentScene->DrawFrame( frameToDraw, c );
                    }
                }
            }
        }

		drwDrawingContext mainContext(this);
		CurrentScene->DrawFrame( Controler->GetCurrentFrame(), mainContext );
	}
	
	// for debugging purpose - display a dot moving every redraw
    //DisplayCounter();
	
	// Draw Output frame
	if( DisplaySettings->GetShowCameraFrame() )
	{
		DrawFrame();
	}
	
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
}

void drwDrawingWidget::timerEvent( QTimerEvent * event )
{
    if( m_drawingEngine )
    {
        m_drawingEngine->Tick();
    }
}

void drwDrawingWidget::DisplayCounter()
{
	static int displayCount = 0;
	
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0.0, this->width(), 0.0, this->height() );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib( GL_CURRENT_BIT );
	
	glColor4d( 1.0, 1.0, 0.0, 1.0 );
	
	int numberOfSteps = (this->width() - 20) / 10;
	int pos = displayCount % numberOfSteps;
	glTranslate( Vec2(pos * 10 + 10, 20) );
	
	glBegin( GL_QUADS );
	{
		glVertex2d( 0.0, 0.0 );
		glVertex2d( 5.0, 0.0 );
		glVertex2d( 5.0, 5.0 );
		glVertex2d( 0.0, 5.0 );
	}
	glEnd();
	
	glPopAttrib();
	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	
	++displayCount;
	
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

void drwDrawingWidget::DrawAllFramesHue()
{
	drwDrawingContext c(this);
	double interval = 1.0;
	if( CurrentScene->GetNumberOfFrames() > 0 )
		interval = 1.0 / CurrentScene->GetNumberOfFrames();
	for( int i = 0; i < CurrentScene->GetNumberOfFrames(); ++i )
	{
		if( i != Controler->GetCurrentFrame() )
		{
            double hue = interval * i;
            QColor color = QColor::fromHslF( hue, 1.0, .5 );
            c.m_colorMultiplier[0] = color.redF();
            c.m_colorMultiplier[1] = color.greenF();
            c.m_colorMultiplier[2] = color.blueF();
            c.m_colorMultiplier[3] = 1.0;
            CurrentScene->DrawFrame( i, c );
		}
	}
}

void drwDrawingWidget::DrawAllFramesRedGreen()
{
	int currentFrame = Controler->GetCurrentFrame();
	int nbFrames = Controler->GetNumberOfFrames();
	if( currentFrame > 0 )
	{
		double interval = .7 / currentFrame;
		drwDrawingContext c(this);
        c.m_colorMultiplier[0] = 1.0;
        c.m_colorMultiplier[1] = 0.0;
        c.m_colorMultiplier[2] = 0.0;
		for( int i = 0; i < currentFrame; ++i )
		{
            c.m_colorMultiplier[0] = .2 + i * interval;
			CurrentScene->DrawFrame( i, c ); 
		}
	}
	int framesAfter = nbFrames - currentFrame;
	if( currentFrame < nbFrames - 1 )
	{
		double interval = .7 / framesAfter;
		drwDrawingContext c(this);
        c.m_colorMultiplier[0] = 0.0;
        c.m_colorMultiplier[1] = 1.0;
        c.m_colorMultiplier[2] = 0.0;
		for( int i = 1; i < framesAfter; ++i )
		{
            c.m_colorMultiplier[1] = .9 - i * interval;
            CurrentScene->DrawFrame( i + currentFrame, c );
		}
	}
}

void drwDrawingWidget::DrawFrame()
{
	// Compute the screen bound in world coordinates
	QSize winSize = size();
	double topLeft[2] = {0.0,0.0};
	theCamera.WindowToWorld( 0, 0, topLeft[0], topLeft[1] );
	double bottomRight[2] = {0.0,0.0};
	theCamera.WindowToWorld( winSize.width(), winSize.height(), bottomRight[0], bottomRight[1] );
	
    double frameTopLeft[2];
    frameTopLeft[0] = 0.0;
    frameTopLeft[1] = CurrentScene->GetFrameHeight();
    double frameBottomRight[2];
    frameBottomRight[0] = CurrentScene->GetFrameWidth();
    frameBottomRight[1] = 0.0;
	
	glColor4d( .3, .3, .3, .6 );
	glBegin( GL_QUADS );
	{
		// Top
		glVertex2d( topLeft[0], topLeft[1] );
		glVertex2d( topLeft[0], frameTopLeft[1] );
		glVertex2d( bottomRight[0], frameTopLeft[1] );
		glVertex2d( bottomRight[0], topLeft[1] );
		// Left
		glVertex2d( topLeft[0], frameTopLeft[1] );
		glVertex2d( topLeft[0], frameBottomRight[1] );
		glVertex2d( frameTopLeft[0], frameBottomRight[1] );
		glVertex2d( frameTopLeft[0], frameTopLeft[1] );
		// Bottom
		glVertex2d( topLeft[0], frameBottomRight[1] );
		glVertex2d( topLeft[0], bottomRight[1] );
		glVertex2d( bottomRight[0], bottomRight[1] );
		glVertex2d( bottomRight[0], frameBottomRight[1] );
		// Right
		glVertex2d( frameBottomRight[0], frameTopLeft[1] );
		glVertex2d( frameBottomRight[0], frameBottomRight[1] );
		glVertex2d( bottomRight[0], frameBottomRight[1] );
		glVertex2d( bottomRight[0], frameTopLeft[1] );
	}
	glEnd();
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

    update();
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

    update();
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

    update();
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

    update();
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
