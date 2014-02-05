#ifndef __drwGLRenderer_h_
#define __drwGLRenderer_h_

#include <QObject>

class Scene;
class Node;
class drwDrawableTexture;
class drwGlslShader;
class Camera;

class drwGLRenderer : public QObject
{

    Q_OBJECT

public:

    explicit drwGLRenderer( QObject * parent = 0 );
    ~drwGLRenderer();

    void Render( int currentFrame, int onionSkinBefore, int onionSkinAfter );
    void RenderToTexture( int currentFrame );
    drwDrawableTexture * GetRenderTexture() { return m_renderTexture; }
    void RenderTextureToScreen();
    void RenderAllFrames( int currentFrame );
    void RenderCameraFrame();

    void SetRenderSize( int width, int height );
    Node * Pick( int currentFrame, int x, int y );
    void SetCurrentScene( Scene * cur );
    drwDrawableTexture * GetWorkTexture() { return m_workTexture; }
    drwGlslShader * GetWidelineShader() { return m_widelineShader; }
    void SetWidelineShader( drwGlslShader * shader ) { m_widelineShader = shader; }
    Camera * GetCamera() { return m_camera; }

    void WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld );
    void WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin );
    double PixelsPerUnit();
    void ShowFullFrame( bool show );
    void SetClearColor( double r, double g, double b, double a );
    
signals:

    void NeedRenderSignal();
    
public slots:

    void RequestRedraw();
    
protected:

    void RenderSetup();
    void DisplayCounter();
    void UpdatePosition( int x, int y );

private:

    Camera              * m_camera;
    Camera              * m_mainCamera;
    Camera              * m_renderToTextureCamera;
    Scene				* CurrentScene;
    drwDrawableTexture  * m_renderTexture;
    drwDrawableTexture  * m_workTexture;
    drwGlslShader       * m_widelineShader;
    bool m_showFullFrame;
    double m_framePadding;
    int m_renderWidth;
    int m_renderHeight;
    double m_clearColor[4];

};

#endif
