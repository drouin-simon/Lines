#include "drwGLRenderer.h"
#include "Scene.h"
#include "drwCamera.h"
#include "drwDrawableTexture.h"
#include "drwDrawingContext.h"
#include "drwGlslShader.h"
#include "Primitive.h"
#include "drwDrawingSurface.h"
#include "Box2i.h"
#include "../GraphicsEngine/include/GraphicsEngineManager.h"
#include <algorithm>

drwGLRenderer::drwGLRenderer()
{
    m_engine = GraphicsEngineManager::getGraphicsEngine();

    // Onion Skin
    m_inOnionFrame = -1;
    m_outOnionFrame = -1;
    m_onionSkinFramesBefore = 1;
    m_onionSkinFramesAfter = 0;
    m_inhibitOnionSkin = false;

    m_renderingEnabled = true;
    m_renderFrame = 0;
    m_overlayModified = true;
    m_sceneModified = true;
    m_drawingSurface = 0;
    m_camera = new drwCamera;
    m_scene = 0;
    m_cursor = 0;
    m_lastShowCursor = false;
    m_showCursor = true;
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

void drwGLRenderer::SetOnionSkinBefore( int value )
{
    m_onionSkinFramesBefore = value;
    NeedRedraw();
}

void drwGLRenderer::SetOnionSkinAfter( int value )
{
    m_onionSkinFramesAfter = value;
    NeedRedraw();
}

void drwGLRenderer::SetInhibitOnionSkin( bool isOn )
{
    m_inhibitOnionSkin = isOn;
    NeedRedraw();
}

void drwGLRenderer::SetInOnionFrame( int f )
{
    m_inOnionFrame = f;
    NeedRedraw();
}

void drwGLRenderer::SetOutOnionFrame( int f )
{
    m_outOnionFrame = f;
    NeedRedraw();
}

void drwGLRenderer::Render()
{
    if( !m_sceneModified && !m_overlayModified )
        return;

    // Render scene to texture
    if( m_sceneModified )
    {
        Box2i modifiedRectWin;
        WorldToGLWindow( m_sceneModifiedRect, modifiedRectWin );

        int onionSkinBefore = GetInhibitOnionSkin() ? 0 : GetOnionSkinBefore();
        int onionSkinAfter = GetInhibitOnionSkin() ? 0 : GetOnionSkinAfter();
        RenderToTexture( m_renderFrame, onionSkinBefore, onionSkinAfter, modifiedRectWin );
        m_sceneModified = false;
        m_sceneModifiedRect.Reset();
    }

    // Adjust overlay modified rect for cursor modification
    if( m_lastShowCursor )
        m_overlayModifiedRect.AdjustBound( m_lastCursorBox );
    if( m_showCursor )
    {
        const Box2d & cursorBBox = m_cursor->BoundingBox();
        m_overlayModifiedRect.AdjustBound( cursorBBox );
        m_lastCursorBox.Init( cursorBBox );
    }
    m_lastShowCursor = m_showCursor;

    // Paste texture to screen
    Box2i overlayModifiedRectWin;
    WorldToGLWindow( m_overlayModifiedRect, overlayModifiedRectWin );
    int x = overlayModifiedRectWin.XMin();
    int y = overlayModifiedRectWin.YMin();
    int w = overlayModifiedRectWin.GetWidth();
    int h = overlayModifiedRectWin.GetHeight();
    RenderTextureToScreen( x, y, w, h );
    m_overlayModified = false;
    m_overlayModifiedRect.Reset();

    // Render cursor
    if( m_cursor && m_showCursor )
    {
        m_engine->enableScissorTest();

        m_engine->scissor(x, y, w, h);

        drwDrawingContext context( this, m_overlayModifiedRect );

        m_cursor->Draw( context );
        m_engine->disableScissorTest();
    }
}

void drwGLRenderer::RenderToTexture( int currentFrame, int onionSkinBefore, int onionSkinAfter, Box2i & rect )
{
    Q_ASSERT( m_scene );

    // State setup
    m_engine->initializeState();
    
    // initialize modelview matrix
    m_engine->initializeModelViewMatrix();
    
    // Resize textures if needed
    int frameSize[2];
    m_camera->GetFrameSizePix( frameSize );
    m_renderTexture->Resize( frameSize[0], frameSize[1] );
    m_layerTexture->Resize( frameSize[0], frameSize[1] );
    m_workTexture->Resize( frameSize[0], frameSize[1] );
    
    // start drawing to texture
    m_renderTexture->DrawToTexture( true );
    
    // Place virtual camera
    m_camera->SetupForFrame();

    // Only update specified region. rect is specified in window coordinate system,
    // we need to change it to frame (texture) coordinate system.
    m_engine->enableScissorTest();
    Box2i frameRect;
    GLWindowToGLFrame( rect, frameRect );
    m_engine->scissor( frameRect.XMin(), frameRect.YMin(), frameRect.GetWidth(), frameRect.GetHeight() );
    
    // Clear texture
    m_engine->clearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], 0.0 );
    m_engine->clear();
    
    // Create a drawing context
    Box2d worldRect;
    GLWindowToWorld( rect, worldRect );
    drwDrawingContext context(this,worldRect);
    
    // Draw in/out frame onions
    if( m_inOnionFrame != -1 && !GetInhibitOnionSkin() )
    {
        context.m_colorMultiplier = Vec4( 0.85, 0.70, 0.0, 1.0 );
        RenderLayer( m_inOnionFrame, context );
    }
    if( m_outOnionFrame != -1 && !GetInhibitOnionSkin() )
    {
        context.m_colorMultiplier = Vec4( 0.0, 0.80, 0.80, 1.0 );
        RenderLayer( m_outOnionFrame, context );
    }

    // Draw onion skins
    int maxFrames = std::max( onionSkinBefore, onionSkinAfter );
    for( int dist = maxFrames; dist > 0; --dist )
    {
        // draw frame before
        if( onionSkinBefore >= dist )
        {
            int f = currentFrame - dist;
            while( f < 0 )
                f = m_scene->GetNumberOfFrames() - abs( f );
            double factor = dist / ( onionSkinBefore + 1.0 );
            factor = exp( -1.5 * factor * factor );
            context.m_colorMultiplier = Vec4( factor, 0.0, 0.0, 1.0 );
            RenderLayer( f, context );
        }
        
        // draw frame after
        if( onionSkinAfter >= dist )
        {
            int f = currentFrame + dist;
            while( f > m_scene->GetNumberOfFrames() - 1 )
                f = f - m_scene->GetNumberOfFrames();
            double factor = dist / ( onionSkinAfter + 1.0 );
            factor = exp( -1.5 * factor * factor );
            context.m_colorMultiplier = Vec4( 0.0, 0.0, factor, 1.0 );
            RenderLayer( f, context );
        }
    }
    
    // draw current frame
    context.m_colorMultiplier = Vec4( 1.0, 1.0, 1.0, 1.0 );
    RenderLayer( currentFrame, context );
    
    // stop drawing to texture
    m_renderTexture->DrawToTexture( false );

    m_engine->disableScissorTest();
}

void drwGLRenderer::RenderToTexture( int currentFrame )
{
    Box2i rect( 0, GetRenderSize()[0] - 1, 0, GetRenderSize()[1] - 1 );
    RenderToTexture( currentFrame, 0, 0, rect );
}

bool drwGLRenderer::IsFrameDisplayed( int frame )
{
    if( GetInhibitOnionSkin() )
    {
        return frame == m_renderFrame;
    }
    else
    {
        int nbFrames = m_scene->GetNumberOfFrames();
        int lastFrame = m_renderFrame + GetOnionSkinAfter();
        int lastFrameClamp = std::max( lastFrame, nbFrames - 1 );
        int lastFrameOver = lastFrame % nbFrames;
        int firstFrame = m_renderFrame - GetOnionSkinBefore();
        int firstFrameClamp = std::max( firstFrame, 0 );
        int firstFrameUnder = nbFrames + firstFrame;
        if( frame >= firstFrameClamp && frame <= lastFrameClamp )
            return true;
        if( frame <= lastFrameOver )
            return true;
        if( frame >= firstFrameUnder )
            return true;
        if( frame == m_inOnionFrame )
            return true;
        if( frame == m_outOnionFrame )
            return true;
        return false;
    }
}

void drwGLRenderer::RenderLayer( int frame, drwDrawingContext & context )
{
    m_layerTexture->DrawToTexture( true );
    m_engine->clearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], 0.0 );
    m_engine->clear();
    m_scene->DrawFrame( frame, context );
    m_layerTexture->DrawToTexture( false );
    
    m_engine->enable(GraphicsEngine::TYPES_MAPPING::BLEND);
    m_engine->blendFunc(GraphicsEngine::TYPES_MAPPING::ONE, GraphicsEngine::TYPES_MAPPING::ONE_MINUS_SRC_ALPHA );
    m_engine->color4d( 1.0, 1.0, 1.0, 1.0 );
    m_layerTexture->PasteToScreen();
}

void drwGLRenderer::RenderTextureToScreen()
{
    RenderTextureToScreen( 0, 0, GetRenderSize()[0], GetRenderSize()[1] );
}

void drwGLRenderer::RenderTextureToScreen( int x, int y, int width, int height )
{
    m_engine->enableScissorTest();
    m_engine->scissor(x, y, width, height);
    
    m_engine->enable(GraphicsEngine::TYPES_MAPPING::BLEND);
    m_engine->blendFunc(GraphicsEngine::TYPES_MAPPING::ONE, GraphicsEngine::TYPES_MAPPING::ONE_MINUS_SRC_ALPHA);
    
    m_engine->clearColor( m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] );
    m_engine->clear();
    
    m_camera->SetupToRenderTextureToScreen();
    m_engine->color4d( 1.0, 1.0, 1.0, 1.0 );
    m_renderTexture->PasteToScreen();

    m_engine->disableScissorTest();
    m_engine->scissor( x, y, width, height );
}

void drwGLRenderer::SetRenderSize( int width, int height )
{
    m_camera->SetWindowSize( width, height );
    NeedRedraw();
}

int * drwGLRenderer::GetRenderSize()
{
    return m_camera->GetWindowSize();
}

void drwGLRenderer::WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld )
{
    m_camera->NativeWindowToWorld( xWin, yWin, xWorld, yWorld );
}

void drwGLRenderer::WindowToWorld( const Box2i & winRect, Box2d & worldRect )
{
    double xMin, yMin;
    WindowToWorld( winRect.XMin(), winRect.YMin(), xMin, yMin );
    double xMax, yMax;
    WindowToWorld( winRect.XMax(), winRect.YMax(), xMax, yMax );
    worldRect.Init( xMin, xMax, yMin, yMax );
}

void drwGLRenderer::GLWindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld )
{
    m_camera->GLWindowToWorld( xWin, yWin, xWorld, yWorld );
}

void drwGLRenderer::GLWindowToWorld( const Box2i & winRect, Box2d & worldRect )
{
    double xMin, yMin;
    GLWindowToWorld( winRect.XMin(), winRect.YMin(), xMin, yMin );
    double xMax, yMax;
    GLWindowToWorld( winRect.XMax(), winRect.YMax(), xMax, yMax );
    worldRect.Init( xMin, xMax, yMin, yMax );
}

void drwGLRenderer::GLWindowToGLFrame( int xwin, int ywin, int & xframe, int & yframe )
{
    m_camera->GLWindowToGLFrame( xwin, ywin, xframe, yframe );
}

void drwGLRenderer::GLWindowToGLFrame( const Box2i & winRect, Box2i & frameRect )
{
    int xMin, yMin;
    GLWindowToGLFrame( winRect.XMin(), winRect.YMin(), xMin, yMin );
    int xMax, yMax;
    GLWindowToGLFrame( winRect.XMax(), winRect.YMax(), xMax, yMax );
    frameRect.Init( xMin, xMax, yMin, yMax );
}

void drwGLRenderer::WorldToGLFrame( double xworld, double yworld, int & xwin, int & ywin )
{
    m_camera->WorldToGLFrame( xworld, yworld, xwin, ywin );
}

void drwGLRenderer::WorldToGLFrame( const Box2d & worldRect, Box2i & winRect )
{
    int xMin, yMin;
    WorldToGLFrame( worldRect.XMin(), worldRect.YMin(), xMin, yMin );
    xMin = std::max( xMin - 1, 0 );
    yMin = std::max( yMin - 1, 0 );
    int xMax, yMax;
    WorldToGLFrame( worldRect.XMax(), worldRect.YMax(), xMax, yMax );
    xMax = std::min( xMax + 1, GetRenderSize()[0] - 1 );
    yMax = std::min( yMax + 1, GetRenderSize()[1] - 1 );
    winRect.Init( xMin, xMax, yMin, yMax );
}

void drwGLRenderer::WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin )
{
    m_camera->WorldToGLWindow( xworld, yworld, xwin, ywin );
}

int clip(int n, int lower, int upper)
{
    return std::max(lower, std::min(n, upper));
}

void drwGLRenderer::WorldToGLWindow( const Box2d & worldRect, Box2i & winRect )
{
    int xMin, yMin;
    WorldToGLWindow( worldRect.XMin(), worldRect.YMin(), xMin, yMin );
    int winXMax = GetRenderSize()[0] - 1;
    int winYMax = GetRenderSize()[1] - 1;
    xMin = clip( xMin - 1, 0, winXMax );
    yMin = clip( yMin - 1, 0, winYMax );
    int xMax, yMax;
    WorldToGLWindow( worldRect.XMax(), worldRect.YMax(), xMax, yMax );
    xMax = clip( xMax + 1, 0, winXMax );
    yMax = clip( yMax + 1, 0, winYMax );
    winRect.Init( xMin, xMax, yMin, yMax );
}

double drwGLRenderer::PixelsPerUnit()
{
    return m_camera->PixelsPerUnit();
}

double drwGLRenderer::UnitsPerPixel()
{
    return m_camera->UnitsPerPixel();
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

void drwGLRenderer::SetRenderFrame( int frame )
{
    if( frame == m_renderFrame )
        return;
    m_renderFrame = frame;
    NeedRedraw();
}

void drwGLRenderer::EnableRendering( bool enable )
{
    m_renderingEnabled = enable;
    if( m_renderingEnabled && m_drawingSurface )
        m_drawingSurface->NeedRedraw();
}

void drwGLRenderer::NeedRedraw()
{
    m_sceneModified = true;
    Box2d modifiedRect( 0.0, m_camera->GetFrameSize()[0], 0.0, m_camera->GetFrameSize()[1] );
    m_sceneModifiedRect.AdjustBound( modifiedRect );
    m_overlayModified = true;
    m_overlayModifiedRect.AdjustBound( modifiedRect );
    if( m_drawingSurface && m_renderingEnabled )
        m_drawingSurface->NeedRedraw();
}

void drwGLRenderer::NeedRedraw( int frame, Box2d & rect )
{
    if( IsFrameDisplayed( frame ) )
    {
        m_sceneModified = true;
        m_sceneModifiedRect.AdjustBound( rect );
        m_overlayModified = true;
        m_overlayModifiedRect.AdjustBound( rect );
        if( m_drawingSurface && m_renderingEnabled )
            m_drawingSurface->NeedRedraw();
    }
}

void drwGLRenderer::MarkOverlayModified()
{
    m_overlayModified = true;
    if( m_drawingSurface && m_renderingEnabled )
        m_drawingSurface->NeedRedraw();
}
