#include "drwGLRenderer.h"
#include "IncludeGl.h"
#include "Scene.h"
#include "Camera.h"
#include "drwDrawableTexture.h"
#include "drwDrawingContext.h"
#include "drwGlslShader.h"

drwGLRenderer::drwGLRenderer(QObject *parent) :
    QObject(parent)
{
    m_camera = new Camera;
    m_renderToTextureCamera = new Camera;
    CurrentScene = 0;
    m_showFullFrame = true;
    m_framePadding = 0.0;
    m_renderTexture = new drwDrawableTexture;
    m_workTexture = new drwDrawableTexture;
    m_widelineShader = 0;
    m_clearColor[0] = 0.0;
    m_clearColor[1] = 0.0;
    m_clearColor[2] = 0.0;
    m_clearColor[3] = 1.0;
}

drwGLRenderer::~drwGLRenderer()
{
    delete m_camera;
    delete m_renderToTextureCamera;
    delete m_renderTexture;
    delete m_workTexture;
    if( m_widelineShader )
        delete m_widelineShader;
}

void drwGLRenderer::RenderSetup()
{
    // State setup
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_LINE_SMOOTH );
    glEnableClientState( GL_VERTEX_ARRAY );
    glClearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_TEXTURE_RECTANGLE_ARB );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // initialize modelview matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    m_camera->SetViewportSize( m_renderWidth, m_renderHeight );
    if( m_showFullFrame )
        m_camera->FitRectInside( CurrentScene->GetFrameWidth(), CurrentScene->GetFrameHeight(), m_framePadding );

    // Update working texture size if needed
    m_workTexture->Resize( m_renderWidth, m_renderHeight );

    // Place virtual camera
    m_camera->PlaceCamera();
}

void drwGLRenderer::Render( int currentFrame, int onionSkinBefore, int onionSkinAfter )
{
    Q_ASSERT( CurrentScene );

    RenderSetup();

    drwDrawingContext c(this);

    Vec4 onionSkinBeforeColor( 1.0, 0.40, 0.40, 1.0 );
    for( int i = 0; i < onionSkinBefore; ++i )
    {
        int frameToDraw = currentFrame - onionSkinBefore + i;
        if( frameToDraw >= 0 )
        {
            double halfDiff = (double)( currentFrame - frameToDraw ) * .2;
            double onionSkinOpacity = 1 / exp( halfDiff );
            c.m_colorMultiplier = onionSkinOpacity * onionSkinBeforeColor;
            c.m_colorMultiplier[3] = 1.0;
            CurrentScene->DrawFrame( frameToDraw, c );
        }
    }

    Vec4 onionSkinAfterColor( 0.0, 0.85, 1.0, 1.0 );
    for( int i = 0; i < onionSkinAfter; ++i )
    {
        int frameToDraw = currentFrame + onionSkinAfter - i;
        if( frameToDraw < CurrentScene->GetNumberOfFrames() - 1 )
        {
            double halfDiff = (double)( frameToDraw - currentFrame ) * .2;
            double onionSkinOpacity = 1 / exp( halfDiff );
            c.m_colorMultiplier = onionSkinOpacity * onionSkinAfterColor;
            c.m_colorMultiplier[3] = 1.0;
            CurrentScene->DrawFrame( frameToDraw, c );
        }
    }

    drwDrawingContext mainContext(this);
    CurrentScene->DrawFrame( currentFrame, mainContext );

    // for debugging purpose - display a dot moving every redraw
    //DisplayCounter();
}

void drwGLRenderer::RenderToTexture( int currentFrame )
{
    // State setup
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_LINE_SMOOTH );
    glEnableClientState( GL_VERTEX_ARRAY );
    glClearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_TEXTURE_RECTANGLE_ARB );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // initialize modelview matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Resize texture if needed
    int texW = 1, texH = 1;
    Camera::RectThatFitsInside( CurrentScene->GetFrameWidth(), CurrentScene->GetFrameHeight(), 0.0, m_renderWidth, m_renderHeight, texW, texH );
    m_renderTexture->Resize( texW, texH );
    m_renderTexture->DrawToTexture( true );

    m_renderToTextureCamera->SetViewportSize( texW, texH );
    m_renderToTextureCamera->FitRectInside( CurrentScene->GetFrameWidth(), CurrentScene->GetFrameHeight(), 0.0 );

    // Update working texture size if needed
    m_workTexture->Resize( texW, texH );

    // Place virtual camera
    m_renderToTextureCamera->PlaceCamera();

    // Do the rendering
    drwDrawingContext mainContext(this);
    CurrentScene->DrawFrame( currentFrame, mainContext );

    // stop drawing to texture
    m_renderTexture->DrawToTexture( false );
}

void drwGLRenderer::FlipAndRender( int frameIndex )
{
    // display previous frame
    m_renderTexture->PasteToScreen();

    // render next frame
    Q_ASSERT( CurrentScene );
    m_renderTexture->DrawToTexture( true );
    RenderSetup();
    drwDrawingContext mainContext(this);
    CurrentScene->DrawFrame( frameIndex, mainContext );
    m_renderTexture->DrawToTexture( false );
}

void drwGLRenderer::RenderAllFrames( int currentFrame )
{
    RenderSetup();

    int nbFrames = CurrentScene->GetNumberOfFrames();
    if( currentFrame > 0 )
    {
        double interval = .7 / currentFrame;
        drwDrawingContext c(this);
        Vec4 onionSkinColor( 1.0, 0.40, 0.40, 1.0 );
        for( int i = 0; i < currentFrame; ++i )
        {
            double factor = .2 + i * interval;
            c.m_colorMultiplier = onionSkinColor * factor;
            c.m_colorMultiplier[3] = 1.0;
            CurrentScene->DrawFrame( i, c );
        }
    }
    int framesAfter = nbFrames - currentFrame;
    if( currentFrame < nbFrames - 1 )
    {
        double interval = .7 / framesAfter;
        drwDrawingContext c(this);
        Vec4 onionSkinColor( 0.0, 0.85, 1.0, 1.0 );
        for( int i = 1; i < framesAfter; ++i )
        {
            double factor = .9 - i * interval;
            c.m_colorMultiplier = onionSkinColor * factor;
            c.m_colorMultiplier[3] = 1.0;
            CurrentScene->DrawFrame( i + currentFrame, c );
        }
    }
}

void drwGLRenderer::RenderCameraFrame()
{
    // Compute the screen bound in world coordinates
    double topLeft[2] = {0.0,0.0};
    m_camera->WindowToWorld( 0, 0, topLeft[0], topLeft[1] );
    double bottomRight[2] = {0.0,0.0};
    m_camera->WindowToWorld( m_renderWidth, m_renderHeight, bottomRight[0], bottomRight[1] );

    double frameTopLeft[2];
    frameTopLeft[0] = 0.0;
    frameTopLeft[1] = CurrentScene->GetFrameHeight();
    double frameBottomRight[2];
    frameBottomRight[0] = CurrentScene->GetFrameWidth();
    frameBottomRight[1] = 0.0;

    glColor4d( .2, .2, .2, .6 );
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

    glColor4d( .25, .25, .25, 1.0 );
    glBegin( GL_LINE_STRIP );
    {
        glVertex2d( frameTopLeft[0], frameTopLeft[1] );
        glVertex2d( frameTopLeft[0], frameBottomRight[1] );
        glVertex2d( frameBottomRight[0], frameBottomRight[1] );
        glVertex2d( frameBottomRight[0], frameTopLeft[1] );
        glVertex2d( frameTopLeft[0], frameTopLeft[1] );
    }
    glEnd();
}

void drwGLRenderer::SetRenderSize( int width, int height )
{
    m_renderWidth = width;
    m_renderHeight = height;
}

void drwGLRenderer::WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld )
{
    m_camera->WindowToWorld( xWin, yWin, xWorld, yWorld );
}

void drwGLRenderer::WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin )
{
    m_camera->WorldToGLWindow( xworld, yworld, xwin, ywin );
}

double drwGLRenderer::PixelsPerUnit()
{
    return m_camera->PixelsPerUnit();
}

void drwGLRenderer::ShowFullFrame( bool show )
{
    m_showFullFrame = show;
    if( m_showFullFrame )
        m_camera->FitRectInside( CurrentScene->GetFrameWidth(), CurrentScene->GetFrameHeight(), m_framePadding );
}

void drwGLRenderer::SetClearColor( double r, double g, double b, double a )
{
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

#define PICK_BUF_SIZE 512
GLuint selectBuf[PICK_BUF_SIZE];

Node * drwGLRenderer::Pick( int currentFrame, int x, int y )
{
    // Enter selection mode and initialize hit buffer
    glSelectBuffer(PICK_BUF_SIZE,selectBuf);
    glRenderMode(GL_SELECT);

    // Place the camera for picking (reduced viewport)
    m_camera->PlaceCameraForPicking( x, y );

    glInitNames();

    // Render
    glLoadIdentity();
    drwDrawingContext context(this);
    context.m_isPicking = true;
    CurrentScene->DrawFrame( currentFrame, context );

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
        return CurrentScene->GetNodeById( currentFrame, id );
    }

    return 0;
}

void drwGLRenderer::SetCurrentScene( Scene * cur )
{
    CurrentScene = cur;
    connect( CurrentScene, SIGNAL( Modified() ), SLOT( RequestRedraw() ), Qt::DirectConnection );
}

void drwGLRenderer::RequestRedraw()
{
    emit NeedRenderSignal();
}

void drwGLRenderer::DisplayCounter()
{
    static int displayCount = 0;

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0.0, m_renderWidth, 0.0, m_renderHeight );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib( GL_CURRENT_BIT );

    glColor4d( 1.0, 1.0, 0.0, 1.0 );

    int numberOfSteps = ( m_renderWidth - 20) / 10;
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


