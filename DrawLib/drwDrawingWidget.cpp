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

drwDrawingWidget::drwDrawingWidget( QWidget * parent ) 
: QGLWidget( parent )
, m_timerId(-1)
, Observer(0)
, CurrentScene(0)
, Controler(0)
, DisplaySettings(0)
, HasDrawn(false)
, m_needUpdateGL(false)
, m_viewportWidget(0)
{
	m_interactor = new drwDrawingWidgetInteractor( this );
	DisplaySettings = new drwDisplaySettings;
	m_workTexture = new drwDrawableTexture;
	connect( DisplaySettings, SIGNAL(ModifiedSignal()), this, SLOT(DisplaySettingsModified()) );
	setAcceptDrops(true);
	setMouseTracking(true);
	setCursor( Qt::BlankCursor );
	setFocusPolicy(Qt::StrongFocus);
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
	updateGL();
}

void drwDrawingWidget::CurrentFrameChanged()
{
	updateGL();
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
	updateGL();
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
}


void drwDrawingWidget::resizeGL( int w, int h )
{
	theCamera.SetViewportSize( w, h );
	m_workTexture->Resize( w, h );
}


void drwDrawingWidget::paintGL()
{
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// needed by line drawing code. simtodo : should not be here.
	glEnable( GL_TEXTURE_RECTANGLE_ARB );
	glEnable( GL_BLEND );     
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
	
	// If first time drawing : position the camera to include frame
	if( !HasDrawn )
	{
		theCamera.FitRectInside( 1920, 1080, .01 );
		HasDrawn = true;
	}
	
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
		
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glPushAttrib( GL_CURRENT_BIT );
		glColor4d( 1.0, 0.0, 0.0, 1.0 );
		CurrentScene->DrawCursor( mainContext );
		glPopAttrib();
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	}
	
	// for debugging purpose - display a dot moving every redraw
    //DisplayCounter();
	
	// Draw Output frame
	if( DisplaySettings->GetShowCameraFrame() )
	{
		DrawFrame();
	}
	
	if( m_viewportWidget )
	{
		m_viewportWidget->Draw();
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
	
	double frameTopLeft[2] = { 0.0, 1080.0 };
	double frameBottomRight[2] = { 1920.0, 0.0 };
	
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
	// try the interactor first
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
	// try the interactor first
	bool interactorSwallows = m_interactor->mouseReleaseEvent( e );
	if( interactorSwallows )
		return;
	
	if ( e->button() == Qt::LeftButton && Observer )
		Observer->MouseReleaseEvent( this, e );
}


void drwDrawingWidget::mouseMoveEvent( QMouseEvent * e )
{
	// try the interactor first
	bool interactorSwallows = m_interactor->mouseMoveEvent( e );
	if( interactorSwallows )
		return;
	
	if ( Observer )
		Observer->MouseMoveEvent( this, e );
}

void drwDrawingWidget::tabletEvent ( QTabletEvent * e )
{
	// try the interactor first
	bool interactorSwallows = m_interactor->tabletEvent( e );
	if( interactorSwallows )
		return;
	
	if( Observer )
		Observer->TabletEvent( this, e );
}

void drwDrawingWidget::keyPressEvent( QKeyEvent * event )
{
	if( event->key() == Qt::Key_Alt )
	{
		QPoint p = this->mapFromGlobal(QCursor::pos());
        m_viewportWidget->Activate( p.x(), p.y() );
		updateGL();
	}
	else
		QGLWidget::keyPressEvent( event );
}

void drwDrawingWidget::keyReleaseEvent( QKeyEvent * event )
{
	if( event->key() == Qt::Key_Alt && m_viewportWidget )
	{
        m_viewportWidget->Deactivate();
		updateGL();
	}
	else
		QGLWidget::keyReleaseEvent( event );
}

void drwDrawingWidget::enterEvent( QEvent * e )
{
	if( Observer )
		Observer->EnterEvent( this, e );
}

void drwDrawingWidget::leaveEvent( QEvent * e )
{
	if( Observer )
		Observer->LeaveEvent( this, e );
    if( m_viewportWidget )
        m_viewportWidget->Deactivate();
    updateGL();
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
	
	// If an updateGL was requested and the controller didn't produce it, trigger updateGL.
	if( m_needUpdateGL && !controlerUpdated )
	{
		updateGL();
	}
	m_needUpdateGL = false;
	
	return QGLWidget::event(e);
}
