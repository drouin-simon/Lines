#ifndef __drwGLRenderer_h_
#define __drwGLRenderer_h_

#include <QObject>

class Scene;
class Node;
class drwDrawableTexture;
class drwGlslShader;
class drwCamera;

class drwGLRenderer : public QObject
{

    Q_OBJECT

public:

    explicit drwGLRenderer( QObject * parent = 0 );
    ~drwGLRenderer();

    void RenderToTexture( int currentFrame, int onionSkinBefore, int onionSkinAfter );
    drwDrawableTexture * GetRenderTexture() { return m_renderTexture; }
    void RenderTextureToScreen( bool transparentBack = false );
    void RenderRect();
    void RenderCameraFrame();

    void SetRenderSize( int width, int height );
    void SetCurrentScene( Scene * cur );
    drwDrawableTexture * GetWorkTexture() { return m_workTexture; }
    drwGlslShader * GetWidelineShader() { return m_widelineShader; }
    void SetWidelineShader( drwGlslShader * shader ) { m_widelineShader = shader; }

    void WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld );
    void WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin );
    double PixelsPerUnit();
    void SetClearColor( double r, double g, double b, double a );
    
signals:

    void NeedRenderSignal();
    
public slots:

    void RequestRedraw();
    
protected:

    drwCamera           * m_camera;
    Scene				* CurrentScene;
    drwDrawableTexture  * m_renderTexture;
    drwDrawableTexture  * m_workTexture;
    drwGlslShader       * m_widelineShader;
    double m_clearColor[4];

};

#endif
