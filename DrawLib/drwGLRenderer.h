#ifndef __drwGLRenderer_h_
#define __drwGLRenderer_h_

#include <QObject>

class Scene;
class Node;
class drwDrawableTexture;
class Camera;
class Box2d;

class drwGLRenderer : public QObject
{

    Q_OBJECT

public:

    explicit drwGLRenderer( QObject * parent = 0 );
    ~drwGLRenderer();

    void Render( int currentFrame, int onionSkinBefore, int onionSkinAfter );
    void RenderAllFrames( int currentFrame );
    void RenderCameraFrame();

    void SetRenderSize( int width, int height );
    Node * Pick( int currentFrame, int x, int y );
    void SetCurrentScene( Scene * cur );
    drwDrawableTexture * GetWorkTexture() { return m_workTexture; }
    Camera * GetCamera() { return m_camera; }

    void WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld );
    void WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin );
    double PixelsPerUnit();
    void ShowFullFrame( bool show );
    
signals:

    void NeedRenderSignal();
    void NeedRenderSignal( int frame, Box2d & modifiedArea );
    
public slots:

    void RequestRedraw();
    void RequestRedraw( int frame, Box2d & modifiedArea );
    
protected:

    void RenderSetup();
    void DisplayCounter();
    void UpdatePosition( int x, int y );

private:

    Camera              * m_camera;
    Scene				* CurrentScene;
    drwDrawableTexture  * m_workTexture;
    bool m_showFullFrame;
    double m_framePadding;
    int m_renderWidth;
    int m_renderHeight;

};

#endif
