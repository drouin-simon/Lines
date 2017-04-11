#include "drwCamera.h"
#include "IncludeGl.h"
#include "math.h"

drwCamera::drwCamera()
{
    // Base vars
    m_winSizePix[0] = 800;
    m_winSizePix[1] = 600;
    m_frameSizeWorld[0] = 1920.0;
    m_frameSizeWorld[1] = 1080.0;
    m_framePadding = 0.0;
    
    // Computed vars -> will be updated in UpdateSize()
    m_frameSizePix[0] = 0;
    m_frameSizePix[1] = 0;
    m_framePosPix[0] = 0;
    m_framePosPix[1] = 0;
    m_winPosWorld[0] = 0.0;
    m_winPosWorld[1] = 0.0;
    m_winSizeWorld[0] = 0.0;
    m_winSizeWorld[1] = 0.0;
    
    UpdateSizes();
}

void drwCamera::SetWindowSize( int w, int h )
{
    m_winSizePix[0] = w;
    m_winSizePix[1] = h;
    UpdateSizes();
}

void drwCamera::SetFrameSize( double w, double h )
{
    m_frameSizeWorld[0] = w;
    m_frameSizeWorld[1] = h;
    UpdateSizes();
}

// WARNING: here, window coord are in window-system coordinate space (y=0 at the top of the window) and
// world coordinates are in OpenGL space (y=0 at the bottom of the window). WorldToGLWindow (next function) is not the inverse operation.
void drwCamera::NativeWindowToWorld( int xwin, int ywin, double & xworld, double & yworld )
{
    xworld = m_winPosWorld[0] + UnitsPerPixel() * xwin;
    yworld = m_winPosWorld[1] + UnitsPerPixel() * ( m_winSizePix[1] - ywin - 1 );
}

// WARNING: here, window coordinates are in the OpenGL window space (y=0 at the bottom of the window)
void drwCamera::GLWindowToWorld( int xwin, int ywin, double & xworld, double & yworld )
{
    xworld = m_winPosWorld[0] + UnitsPerPixel() * xwin;
    yworld = m_winPosWorld[1] + UnitsPerPixel() * ywin;
}

void drwCamera::GLWindowToGLFrame( int xwin, int ywin, int & xframe, int & yframe )
{
    xframe = xwin - m_framePosPix[0];
    yframe = ywin - m_framePosPix[1];
}

void drwCamera::WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin )
{
    double scaleFactor = PixelsPerUnit();
    xwin = m_framePosPix[0] + (int)round( scaleFactor * xworld );
    ywin = m_framePosPix[1] + (int)round( scaleFactor * yworld );
}

// WARNING: here, window coordinates are in the OpenGL window space (y=0 at the bottom of the window)
// this is not the inverse of the previous function (WindowToWorld)
void drwCamera::WorldToGLFrame( double xworld, double yworld, int & xwin, int & ywin )
{
    double scaleFactor = PixelsPerUnit();
    xwin = (int)round( scaleFactor * xworld );
    ywin = (int)round( scaleFactor * yworld );
}

double drwCamera::PixelsPerUnit()
{
    return m_frameSizePix[0] / m_frameSizeWorld[0];
}

double drwCamera::UnitsPerPixel()
{
    return m_frameSizeWorld[0] / m_frameSizePix[0] ;
}

void drwCamera::SetupForWindow()
{
    glViewport( 0, 0, m_winSizePix[0], m_winSizePix[1] );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D( m_winPosWorld[0], m_winPosWorld[0] + m_winSizeWorld[0], m_winPosWorld[1], m_winPosWorld[1] + m_winSizeWorld[1] );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drwCamera::SetupForFrame()
{
    glViewport( 0, 0, m_frameSizePix[0], m_frameSizePix[1] );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D( 0.0, m_frameSizeWorld[0], 0.0, m_frameSizeWorld[1] );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drwCamera::SetupToRenderTextureToScreen()
{
    glViewport( m_framePosPix[0], m_framePosPix[1], m_frameSizePix[0], m_frameSizePix[1] );
}

#include <iostream>

void drwCamera::Print()
{
    std::cout << "Window size " << m_winSizePix[0] << " x " << m_winSizePix[1] << " pix" << std::endl;
    std::cout << "Frame size (world): " << m_frameSizeWorld[0] << " x " << m_frameSizeWorld[1] << " units" << std::endl;
    std::cout << "Frame padding: " << m_framePadding << std::endl;
    std::cout << "Frame size (pix): " << m_frameSizePix[0] << " x " << m_frameSizePix[1] << std::endl;
    std::cout << "Frame pos (pix): ( " << m_framePosPix[0] << ", " << m_framePosPix[1] << " )" << std::endl;
}

void drwCamera::UpdateSizes()
{
    // Compute m_frameSizePix : find the rectangle with same proportion as the frame that fits inside the window
    RectThatFitsInside( m_frameSizeWorld[0], m_frameSizeWorld[1], m_framePadding, m_winSizePix[0], m_winSizePix[1], m_frameSizePix[0], m_frameSizePix[1] );
    
    // Compute frame pos in window
    m_framePosPix[0] = ( m_winSizePix[0] - m_frameSizePix[0] ) / 2;
    m_framePosPix[1] = ( m_winSizePix[1] - m_frameSizePix[1] ) / 2;
    
    // Compute the params of window in world space
    double upp = UnitsPerPixel();
    m_winPosWorld[0] = -1.0 * m_framePosPix[0] * upp;
    m_winPosWorld[1] = -1.0 * m_framePosPix[1] * upp;
    m_winSizeWorld[0] = m_winSizePix[0] * upp;
    m_winSizeWorld[1] = m_winSizePix[1] * upp;
    
}

// find the the size of rectangle (rectW,rectH) that has the same proportion
// as (w,h) and that fits inside (winW,winH) with a certain border around it
// given by percentBorder
void drwCamera::RectThatFitsInside( double w, double h, double percentBorder, int winW, int winH, int & rectW, int & rectH )
{
    double ratioRect = w / h;
    double ratioWin = (double)winW / winH;
    if( ratioRect > ratioWin )
    {
        // the width matters
        rectW = winW - 2 * percentBorder * winW;
        rectH = rectW / ratioRect;
    }
    else
    {
        // the height matters
        rectH = winH - 2 * percentBorder * winH;
        rectW = rectH * ratioRect;
    }
}
