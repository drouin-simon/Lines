#include "Camera.h"
#include "IncludeGl.h"

Camera::Camera()
{
    m_windowW                   = 800;
    m_windowH                   = 600;
    m_posX                      = 0.0;
    m_posY                      = 0.0;
    m_virtWindowH               = 300;
    AdjustFrustum();
}


void Camera::WindowToWorld( int xwin, int ywin, double & xworld, double & yworld )
{
    double ratio = (double)m_windowW / (double)m_windowH;
    double virtWindowW = ratio * m_virtWindowH;
    xworld = m_posX + ( (double)xwin - (double)m_windowW/(double)2.0 ) * virtWindowW/m_windowW;
    yworld = m_posY + ( (double)m_windowH/(double)2.0 - (double)ywin ) * m_virtWindowH/m_windowH;
}


void Camera::SetViewportSize( int w, int h )
{
    //m_virtWindowH *= (double)h / (double) m_windowH;
    m_windowW = w;
    m_windowH = h;
    glViewport( 0, 0, (GLint)w, (GLint)h );
    AdjustFrustum();
}


void Camera::Pan( double xPercent, double yPercent )
{
    m_posX += m_virtWindowH * xPercent;
    m_posY += m_virtWindowH * yPercent;
    AdjustFrustum();
}


void Camera::Zoom( double factor )
{
    m_virtWindowH *= factor;
    AdjustFrustum();
}

void Camera::PlaceCamera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D( m_left, m_right, m_bottom, m_top );
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Camera::PlaceCameraForPicking( int x, int y )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	gluPickMatrix(x,viewport[3]-y,5,5,viewport);
	gluOrtho2D( m_left, m_right, m_bottom, m_top );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Camera::AdjustFrustum()
{
    double ratio = (double)m_windowW / (double) m_windowH;
    double virtWindowW =  ratio * m_virtWindowH;
    double radiusX = virtWindowW/(double)2.0;
    double radiusY = m_virtWindowH/(double)2.0;
    m_left = m_posX - radiusX;
    m_right = m_posX + radiusX;
    m_top = m_posY + radiusY;
    m_bottom = m_posY - radiusY;
}

void Camera::FitRectInside( double w, double h, double percentBorder )
{
	double ratioRect = w / h;
	double ratioWin = (double)m_windowW / m_windowH;
	double virtWindowW = 1.0;
	if( ratioRect > ratioWin )
	{
		// the width matters
		virtWindowW = w + 2 * percentBorder * w;
		m_virtWindowH = virtWindowW / ratioWin;
	}
	else
	{
		// the height matters
		m_virtWindowH = h + 2 * percentBorder * h;
		virtWindowW = m_virtWindowH * ratioWin;
	}
	m_posX = w / 2;
	m_posY = h / 2;
	AdjustFrustum();
}
