#ifndef __drwGLRenderer_h_
#define __drwGLRenderer_h_

#include "Box2d.h"

class Scene;
class Node;
class Primitive;
class drwDrawableTexture;
class drwGlslShader;
class drwCamera;
class drwDrawingContext;
class drwDrawingSurface;
class Box2i;

class drwGLRenderer
{

public:

    explicit drwGLRenderer();
    ~drwGLRenderer();

    void SetDrawingSurface( drwDrawingSurface * s ) { m_drawingSurface = s; }
    void SetCursor( Primitive * p ) { m_cursor = p; }
    void SetShowCursor( bool show ) { m_showCursor = show; }

    // Onion Skins
    int GetOnionSkinBefore() { return m_onionSkinFramesBefore; }
    void SetOnionSkinBefore( int );
    int GetOnionSkinAfter() { return m_onionSkinFramesAfter; }
    void SetOnionSkinAfter( int );
    bool GetInhibitOnionSkin() { return m_inhibitOnionSkin; }
    void SetInhibitOnionSkin( bool isOn );
    int GetInOnionFrame() { return m_inOnionFrame; }
    void SetInOnionFrame( int f );
    int GetOutOnionFrame() { return m_outOnionFrame; }
    void SetOutOnionFrame( int f );

    void Render();
    void RenderToTexture( int currentFrame, int onionSkinBefore, int onionSkinAfter, Box2i & rect );
    void RenderToTexture( int currentFrame );
    drwDrawableTexture * GetRenderTexture() { return m_renderTexture; }
    void RenderTextureToScreen();
    void RenderTextureToScreen( int x, int y, int width, int height );

    void SetRenderSize( int width, int height );
    int * GetRenderSize();
    void SetCurrentScene( Scene * cur );
    void SetRenderFrame( int frame );
    drwDrawableTexture * GetWorkTexture() { return m_workTexture; }
    drwGlslShader * GetWidelineShader() { return m_widelineShader; }
    void SetWidelineShader( drwGlslShader * shader ) { m_widelineShader = shader; }

    void WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld );
    void WindowToWorld( const Box2i & winRect, Box2d & worldRect );
    void GLWindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld );
    void GLWindowToWorld( const Box2i & winRect, Box2d & worldRect );
    void GLWindowToGLFrame( int xwin, int ywin, int & xframe, int & yframe );
    void GLWindowToGLFrame( const Box2i & winRect, Box2i & frameRect );
    void WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin );
    void WorldToGLWindow( const Box2d & worldRect, Box2i & winRect );
    void WorldToGLFrame( double xworld, double yworld, int & xwin, int & ywin );
    void WorldToGLFrame( const Box2d & worldRect, Box2i & winRect );
    double PixelsPerUnit();
    double UnitsPerPixel();
    void SetClearColor( double r, double g, double b, double a );

    void EnableRendering( bool enable );
    void NeedRedraw();
    void NeedRedraw( int frame, Box2d & rect );
    void MarkOverlayModified();
    
protected:

    bool IsFrameDisplayed( int frame );
    void RenderLayer( int frame, drwDrawingContext & context );

    // Onion skin params
    int m_inOnionFrame;
    int m_outOnionFrame;
    int m_onionSkinFramesBefore;
    int m_onionSkinFramesAfter;
    bool m_inhibitOnionSkin;

    bool m_overlayModified;
    Box2d m_overlayModifiedRect;

    bool m_renderingEnabled;
    bool m_sceneModified;
    Box2d m_sceneModifiedRect;
    int m_renderFrame;

    // Cursor
    Primitive * m_cursor;
    bool        m_showCursor;
    bool        m_lastShowCursor;
    Box2d       m_lastCursorBox;

    drwDrawingSurface   * m_drawingSurface;
    drwCamera           * m_camera;
    Scene				* m_scene;
    drwDrawableTexture  * m_renderTexture;
    drwDrawableTexture  * m_layerTexture;
    drwDrawableTexture  * m_workTexture;
    drwGlslShader       * m_widelineShader;
    double m_clearColor[4];

};

#endif
