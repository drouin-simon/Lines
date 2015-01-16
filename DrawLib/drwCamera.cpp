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
void drwCamera::WindowToWorld( int xwin, int ywin, double & xworld, double & yworld )
{
    xworld = m_winPosWorld[0] + UnitsPerPixel() * xwin;
    yworld = m_winPosWorld[1] + UnitsPerPixel() * ( m_winSizePix[1] - ywin - 1 );
}

// WARNING: here, window coordinates are in the OpenGL window space (y=0 at the bottom of the window)
// this is not the inverse of the previous function (WindowToWorld)
void drwCamera::WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin )
{
    double scaleFactor = PixelsPerUnit();
    xwin = (int)round( scaleFactor * xworld );
    ywin = (int)round( scaleFactor * xworld );
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

// This function is assuming that SetupForWindow has been called before
void drwCamera::RenderCameraFrame()
{
    // Compute the screen bound in world coordinates
    double topLeft[2] = { 0.0,0.0};
    topLeft[0] = m_winPosWorld[0];
    topLeft[1] = m_winPosWorld[1] + m_winSizeWorld[1];
    double bottomRight[2] = {0.0,0.0};
    bottomRight[0] = m_winPosWorld[0] + m_winSizeWorld[0];
    bottomRight[1] = m_winPosWorld[1];
    
    double frameTopLeft[2];
    frameTopLeft[0] = 0.0;
    frameTopLeft[1] = m_frameSizeWorld[1];
    double frameBottomRight[2];
    frameBottomRight[0] = m_frameSizeWorld[0];
    frameBottomRight[1] = 0.0;
    
    glColor4d( .2, .2, .2, .6 );
    glBegin( GL_QUADS );
    {
        // Top
        glVertex2d( topLeft[0], topLeft[1] );
        glVertex2d( topLeft[0], frameTopLeft[1] );
        glVertex2d( bottomRight[0], frameTopLeft[1] );
        glVertex2d( bottomRight[0], topLeft[1] );
        // Left
        glVertex2d( topLeft[0], frameTopLeft[1] );
        glVertex2d( topLeft[0], frameBottomRight[1] );
        glVertex2d( frameTopLeft[0], frameBottomRight[1] );
        glVertex2d( frameTopLeft[0], frameTopLeft[1] );
        // Bottom
        glVertex2d( topLeft[0], frameBottomRight[1] );
        glVertex2d( topLeft[0], bottomRight[1] );
        glVertex2d( bottomRight[0], bottomRight[1] );
        glVertex2d( bottomRight[0], frameBottomRight[1] );
        // Right
        glVertex2d( frameBottomRight[0], frameTopLeft[1] );
        glVertex2d( frameBottomRight[0], frameBottomRight[1] );
        glVertex2d( bottomRight[0], frameBottomRight[1] );
        glVertex2d( bottomRight[0], frameTopLeft[1] );
    }
    glEnd();
    
    glColor4d( .25, .25, .25, 1.0 );
    glBegin( GL_LINE_STRIP );
    {
        glVertex2d( frameTopLeft[0], frameTopLeft[1] );
        glVertex2d( frameTopLeft[0], frameBottomRight[1] );
        glVertex2d( frameBottomRight[0], frameBottomRight[1] );
        glVertex2d( frameBottomRight[0], frameTopLeft[1] );
        glVertex2d( frameTopLeft[0], frameTopLeft[1] );
    }
    glEnd();
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
        rectW = winW + 2 * percentBorder * winW;
        rectH = rectW / ratioRect;
    }
    else
    {
        // the height matters
        rectH = winH + 2 * percentBorder * winH;
        rectW = rectH * ratioRect;
    }
}
