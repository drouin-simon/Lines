#ifndef __drwGLRenderer_h_
#define __drwGLRenderer_h_

class Scene;
class Node;
class drwDrawableTexture;
class drwGlslShader;
class drwCamera;
class drwDrawingContext;
class drwDrawingSurface;

class drwGLRenderer
{

public:

    explicit drwGLRenderer();
    ~drwGLRenderer();

    void SetDrawingSurface( drwDrawingSurface * s ) { m_drawingSurface = s; }

    void RenderToTexture( int currentFrame, int onionSkinBefore, int onionSkinAfter );
    drwDrawableTexture * GetRenderTexture() { return m_renderTexture; }
    void RenderTextureToScreen();
    void RenderTextureToScreen( int x, int y, int width, int height );
    void RenderRect();
    void RenderCameraFrame();

    void SetRenderSize( int width, int height );
    int * GetRenderSize();
    void SetCurrentScene( Scene * cur );
    drwDrawableTexture * GetWorkTexture() { return m_workTexture; }
    drwGlslShader * GetWidelineShader() { return m_widelineShader; }
    void SetWidelineShader( drwGlslShader * shader ) { m_widelineShader = shader; }

    void WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld );
    void WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin );
    double PixelsPerUnit();
    void SetClearColor( double r, double g, double b, double a );

    void NeedRedraw();
    void NeedRedraw( int x, int y, int width, int height );
    
protected:

    void RenderLayer( int frame, drwDrawingContext & context );

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
