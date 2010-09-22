#include "drwDrawingWidget.h"
#include <QtGui>
#include <QtOpenGL>

#include "Scene.h"
#include "drwWidgetObserver.h"
#include "PlaybackControler.h"
#include "drwDisplaySettings.h"
#include "drwDrawingContext.h"
#include "drwDrawableTexture.h"

drwCommand::s_ptr drwDrawingWidget::CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e )
{
	double xWin = (double)e->x();
	double yWin = (double)e->y();
	double xWorld = 0.0;
	double yWorld = 0.0;
	theCamera.WindowToWorld( xWin, yWin, xWorld, yWorld );
	drwCommand::s_ptr command( new drwMouseCommand( 0, commandType, xWorld, yWorld, 0.0, 0, 0, 1.0, 0.0, 0.0 ) );
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
	drwCommand::s_ptr command( new drwMouseCommand( 0, commandType, xWorld, yWorld, 0.0, e->xTilt(), e->yTilt(), e->pressure(), e->rotation(), e->tangentialPressure() ) );
	return command;
}


void drwDrawingWidget::WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld )
{
	theCamera.WindowToWorld( xWin, yWin, xWorld, yWorld );
}


drwDrawingWidget::drwDrawingWidget( QWidget * parent ) 
: QGLWidget( /*QGLFormat(QGL::SampleBuffers),*/ parent )
, m_timerId(-1)
, currentMouseX(0)
, currentMouseY(0)
, lastMouseX(0)
, lastMouseY(0)
, mouseMoving(false)
, isScaling(false)
, isPaning(false)
, Observer(0)
, CurrentScene(0)
, Controler(0)
, DisplaySettings(0)
, HasDrawn(false)
, m_needUpdateGL(false)
{
	DisplaySettings = new drwDisplaySettings;
	m_workTexture = new drwDrawableTexture;
	connect( DisplaySettings, SIGNAL(ModifiedSignal()), this, SLOT(DisplaySettingsModified()) );
	setAcceptDrops(true);
	setMouseTracking(true);
}

void drwDrawingWidget::SetControler( PlaybackControler * controler )
{
	Controler = controler;
	connect(Controler, SIGNAL(FrameChanged(int)), SLOT(CurrentFrameChanged()) );
	connect(Controler, SIGNAL(StartStop(bool)), SLOT(PlaybackStartStop(bool)) );
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
	// If we are in the main thread, repaint as soon as possible. If not, it can wait until the next idle moment
	if( QApplication::instance()->thread() == QThread::currentThread() )
		updateGL();
	else
	{
		// post a paint event
		QMetaObject::invokeMethod(this, "updateGL", Qt::AutoConnection);
	}
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
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_DEPTH_TEST );
	
	// Enable antialiasing of lines
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_BLEND );     
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
	
	// Enable global antialiasing
	/*glEnable(GL_MULTISAMPLE);
	GLint maxSamples;
	glGetIntegerv(GL_MAX_SAMPLES_EXT, &maxSamples);
	format().setSamples( maxSamples );*/

	// Enable vertex arrays: needed by geometry classes
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
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
	
	// If first time drawing : position the camera to include frame
	if( !HasDrawn )
	{
		theCamera.FitRectInside( 1920, 1080, .05 );
		HasDrawn = true;
	}
	
	// Place virtual camera
	theCamera.PlaceCamera();
	glLoadIdentity();
	
	if( CurrentScene )
	{
		// Draw the Scene
		drwDrawingContext persistentContext(this);
		CurrentScene->DrawPersistent( persistentContext );
		if( DisplaySettings->GetShowAllFrames() )
		{
			//DrawAllFramesHue();
			DrawAllFramesRedGreen();
		}
		else if( !DisplaySettings->GetInhibitOnionSkin() )
		{
			drwDrawingContext c(this);
			c.m_opacity = .4;
			for( int i = 0; i < DisplaySettings->GetOnionSkinBefore(); ++i )
				if( Controler->GetCurrentFrame() - i - 1 >= 0 )
					CurrentScene->DrawFrame( Controler->GetCurrentFrame() - i - 1, c );
			for( int i = 0; i < DisplaySettings->GetOnionSkinAfter(); ++i )
				if( Controler->GetCurrentFrame() + i + 1 < CurrentScene->GetNumberOfFrames() - 1 )
					CurrentScene->DrawFrame( Controler->GetCurrentFrame() + i + 1, c );
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
	DisplayCounter();
	
	// Draw Output frame
	if( DisplaySettings->GetShowCameraFrame() )
	{
		DrawFrame();
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
	c.m_isOverridingColor = true;
	double interval = 1.0;
	if( CurrentScene->GetNumberOfFrames() > 0 )
		interval = 1.0 / CurrentScene->GetNumberOfFrames();
	for( int i = 0; i < CurrentScene->GetNumberOfFrames(); ++i )
	{
		if( i != Controler->GetCurrentFrame() )
		{
			double hue = interval * i;
			QColor color = QColor::fromHslF( hue, 1.0, .5 );
			c.m_colorOverride[0] = color.redF();
			c.m_colorOverride[1] = color.greenF();
			c.m_colorOverride[2] = color.blueF();
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
		c.m_isOverridingColor = true;
		c.m_colorOverride[0] = 1.0;
		c.m_colorOverride[1] = 0.0;
		c.m_colorOverride[2] = 0.0;
		for( int i = 0; i < currentFrame; ++i )
		{
			c.m_opacity = .2 + i * interval;
			CurrentScene->DrawFrame( i, c ); 
		}
	}
	int framesAfter = nbFrames - currentFrame;
	if( currentFrame < nbFrames - 1 )
	{
		double interval = .7 / framesAfter;
		drwDrawingContext c(this);
		c.m_isOverridingColor = true;
		c.m_colorOverride[0] = 0.0;
		c.m_colorOverride[1] = 1.0;
		c.m_colorOverride[2] = 0.0;
		for( int i = 1; i < framesAfter; ++i )
		{
			c.m_opacity = .9 - i * interval;
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
	if ( e->button() == Qt::LeftButton )
	{
		mouseMoving = true;
		currentMouseX  = e->x();
		currentMouseY  = e->y();

		if( e->modifiers() & Qt::ShiftModifier )
		{
			isPaning = true;
		}
		else if ( e->modifiers() & Qt::ControlModifier || e->modifiers() & Qt::AltModifier )
		{
			isScaling = true;
		}
		else if( Observer )
		{
			Observer->MousePressEvent( this, e );
		}
	}
}


void drwDrawingWidget::mouseReleaseEvent( QMouseEvent * e )
{
	if ( e->button() == Qt::LeftButton )
	{
		if( !isPaning && !isScaling )
			Observer->MouseReleaseEvent( this, e );

		// cancel all operation on view
		mouseMoving = false;
		isPaning    = false;
		isScaling   = false;
	}
}


void drwDrawingWidget::mouseMoveEvent( QMouseEvent * e )
{
	lastMouseX = currentMouseX;
	lastMouseY = currentMouseY;
	currentMouseX = e->x();
	currentMouseY = e->y();

	bool selfInducedRedraw = false;
	if( isScaling )
	{
		theCamera.Zoom( 1.0 + (double)(currentMouseY - lastMouseY) / (double)100.0 );
		selfInducedRedraw = true;
	}
	else if( isPaning )
	{
		double xPercent = -1.0 * (double)( currentMouseX - lastMouseX ) / (double)( theCamera.GetWindowWidth() );
		double yPercent = (double)( currentMouseY - lastMouseY ) / (double)( theCamera.GetWindowHeight() );
		theCamera.Pan( xPercent, yPercent );
		selfInducedRedraw = true;
	}
	else
	{
		Observer->MouseMoveEvent( this, e );
	}
	
	if( selfInducedRedraw )
		updateGL();
}

void drwDrawingWidget::tabletEvent ( QTabletEvent * e )
{
	bool isPress = e->type() == QEvent::TabletPress;
	bool isModifier = e->modifiers() & Qt::ControlModifier || e->modifiers() & Qt::AltModifier || e->modifiers() & Qt::ShiftModifier;
	bool viewManipulation = false;
	viewManipulation |= isPress && isModifier;
	viewManipulation |= isPaning;
	viewManipulation |= isScaling;
	
	if( !viewManipulation )
		Observer->TabletEvent( this, e );
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
