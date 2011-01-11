#ifndef CAMERA_H
#define CAMERA_H

class Camera
{

public:

    Camera();

    int GetWindowHeight() { return m_windowH; }
    int GetWindowWidth()  { return m_windowW; }

    void WindowToWorld( int xwin, int ywin, double & xworld, double & yworld );
    void WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin );

    void SetViewportSize( int w, int h );

    void Pan( double xPercent, double yPercent );
    void Zoom( double factor );

    void PlaceCamera();
    void PlaceCameraForPicking( int x, int y );
	
	void FitRectInside( double w, double h, double percentBorder );

private:

    void AdjustFrustum();

    double m_left;
    double m_right;
    double m_bottom;
    double m_top;

    double m_posX;
    double m_posY;
    double m_virtWindowH;

    int m_windowW;
    int m_windowH;

};


#endif
