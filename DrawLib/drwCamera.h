#ifndef __drwCamera_h_
#define __drwCamera_h_

class drwCamera
{

public:

    drwCamera();
    
    void SetWindowSize( int w, int h );
    int * GetWindowSize() { return m_winSizePix; }
    void SetFrameSize( double w, double h );
    void GetFrameSizePix( double frameSize[2] ) { frameSize[0] = m_frameSizePix[0]; frameSize[1] = m_frameSizePix[1]; }

    void WindowToWorld( int xwin, int ywin, double & xworld, double & yworld );
    void WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin );
    double PixelsPerUnit();
    double UnitsPerPixel();

    void SetupForWindow();
    void SetupForFrame();
    void SetupToRenderTextureToScreen();
    
    void RenderCameraFrame();
	    
private:
    
    void UpdateSizes();
    void RectThatFitsInside( double w, double h, double percentBorder, int winW, int winH, int & rectW, int & rectH );
    
    // basic variables from which everything else is computed
    // size of the frame in pixels can be computed by finding the
    // largest rectangle with proportions similar to the frame that
    // fits in the window.
    int m_winSizePix[2];         // total window size in pixels
    double m_frameSizeWorld[2];  // frame size in world coordinates
    double m_framePadding;
    
    // Computed variables
    int m_frameSizePix[2];
    int m_framePosPix[2];
    double m_winPosWorld[2];    // position of the bottom left corner of the window in world space
    double m_winSizeWorld[2];   // size of the window in world coordinates.
    
};


#endif
