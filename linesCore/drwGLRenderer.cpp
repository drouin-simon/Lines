#include "drwGLRenderer.h"
#include "IncludeGl.h"
#include "Scene.h"
#include "drwCamera.h"
#include "drwDrawableTexture.h"
#include "drwDrawingContext.h"
#include "drwGlslShader.h"
#include "drwDrawingSurface.h"

drwGLRenderer::drwGLRenderer()
{
    m_drawingSurface = 0;
    m_camera = new drwCamera;
    m_scene = 0;
    m_renderTexture = new drwDrawableTexture;
    m_layerTexture = new drwDrawableTexture;
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
    delete m_renderTexture;
    delete m_layerTexture;
    delete m_workTexture;
    if( m_widelineShader )
        delete m_widelineShader;
}

void drwGLRenderer::RenderToTexture( int currentFrame, int onionSkinBefore, int onionSkinAfter )
{
    Q_ASSERT( m_scene );
    
    // State setup
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_LINE_SMOOTH );
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnable( GL_TEXTURE_RECTANGLE_ARB );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    // initialize modelview matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    // Resize textures if needed
    double frameSize[2];
    m_camera->GetFrameSizePix( frameSize );
    m_renderTexture->Resize( frameSize[0], frameSize[1] );
    m_layerTexture->Resize( frameSize[0], frameSize[1] );
    m_workTexture->Resize( frameSize[0], frameSize[1] );
    
    // start drawing to texture
    m_renderTexture->DrawToTexture( true );
    
    // Place virtual camera
    m_camera->SetupForFrame();
    
    // Clear texture
    glClearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], 0.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // Create a drawing context
    drwDrawingContext context(this);
    
    int maxFrames = std::max( onionSkinBefore, onionSkinAfter );
    for( int dist = maxFrames; dist > 0; --dist )
    {
        // draw frame before
        if( onionSkinBefore >= dist )
        {
            int f = currentFrame - dist;
            while( f < 0 )
                f = m_scene->GetNumberOfFrames() - abs( f );
            RenderLayer( f, context );
        }
        
        // draw frame after
        if( onionSkinAfter >= dist )
        {
            int f = currentFrame + dist;
            while( f > m_scene->GetNumberOfFrames() - 1 )
                f = f - m_scene->GetNumberOfFrames();
            RenderLayer( f, context );
        }
        
        // draw background of next layer
        glColor4d( m_clearColor[0], m_clearColor[1], m_clearColor[2], 0.5 );
        RenderRect();
    }
    
    // draw current frame
    RenderLayer( currentFrame, context );
    
    // stop drawing to texture
    m_renderTexture->DrawToTexture( false );
}

void drwGLRenderer::RenderLayer( int frame, drwDrawingContext & context )
{
    m_layerTexture->DrawToTexture( true );
    glClearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], 0.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_scene->DrawFrame( frame, context );
    m_layerTexture->DrawToTexture( false );
    
    glEnable( GL_BLEND );
    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
    glColor4d( 1.0, 1.0, 1.0, 1.0 );
    m_layerTexture->PasteToScreen();
}

void drwGLRenderer::RenderTextureToScreen()
{
    RenderTextureToScreen( 0, 0, GetRenderSize()[0], GetRenderSize()[1] );
}

void drwGLRenderer::RenderTextureToScreen( int x, int y, int width, int height )
{
    glEnable( GL_SCISSOR_TEST );
    glScissor( x, y, width, height );

    glEnable( GL_BLEND );
    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
    
    glClearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    m_camera->SetupToRenderTextureToScreen();
    glColor4d( 1.0, 1.0, 1.0, 1.0 );
    m_renderTexture->PasteToScreen();

    glDisable( GL_SCISSOR_TEST );
    glScissor( x, y, width, height );
}

void drwGLRenderer::RenderRect()
{
    glPushAttrib( GL_COLOR_BUFFER_BIT );

    glEnable( GL_BLEND );
    glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    
    double tw = m_renderTexture->GetWidth();
    double th = m_renderTexture->GetHeight();
    gluOrtho2D( 0, tw, 0, th );
    
    glBegin( GL_QUADS );
    {
        glVertex2d( 0.0, 0.0 );
        glVertex2d( tw, 0.0 );
        glVertex2d( tw, th );
        glVertex2d( 0.0, th );
    }
    glEnd();
    
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    glPopAttrib();
}

void drwGLRenderer::RenderCameraFrame()
{
    m_camera->SetupForWindow();
    m_camera->RenderCameraFrame();
}

void drwGLRenderer::SetRenderSize( int width, int height )
{
    m_camera->SetWindowSize( width, height );
}

int * drwGLRenderer::GetRenderSize()
{
    return m_camera->GetWindowSize();
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

void drwGLRenderer::SetClearColor( double r, double g, double b, double a )
{
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

void drwGLRenderer::SetCurrentScene( Scene * s )
{
    m_scene = s;
    m_scene->SetRenderer( this );
}

void drwGLRenderer::NeedRedraw()
{
    m_drawingSurface->NeedRedraw();
}

void drwGLRenderer::NeedRedraw( int x, int y, int width, int height )
{
    m_drawingSurface->NeedRedraw( x, y, width, height );
}
