#include "drwDrawableTexture.h"
#include "IncludeGLee.h"
#include "IncludeGl.h"

drwDrawableTexture::drwDrawableTexture()
	: m_texId(0)
	, m_fbId(0)
	, m_width(1)
	, m_height(1)
{
}

drwDrawableTexture::~drwDrawableTexture()
{
	Release();
}

bool drwDrawableTexture::Init( int width, int height )
{
	m_width = width;
	m_height = height;

	// init texture
	glGenTextures( 1, &m_texId );
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_texId );
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, 2, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 0 );
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0 );

	// Init framebuffer
	bool success = true;
	glGenFramebuffersEXT( 1, &m_fbId );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_fbId );
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, m_texId, 0 );

	GLenum ret = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
	if( ret != GL_FRAMEBUFFER_COMPLETE_EXT )
		success = false;

	// clear the texture
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );

	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

	return success;
}

void drwDrawableTexture::Resize( int width, int height )
{
	if( m_texId && ( m_width != width || m_height != height ) )
	{
		glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_texId );
		glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, 2, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 0 );
		glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
	}
}

void drwDrawableTexture::Release()
{
	if( m_fbId )
		glDeleteFramebuffersEXT( 1, &m_fbId );
	if( m_texId )
		glDeleteTextures( 1, &m_texId );
}

void drwDrawableTexture::DrawToTexture( bool drawTo )
{
	if( drawTo && m_fbId )
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_fbId );
	else
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}

// Paste the content of a sub-rectangle of
// the texture on the screen, assuming the screen is the
// same size as the texture. If it is not the case, scaling
// will happen.
void drwDrawableTexture::PasteToScreen( int x, int y, int width, int height )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0, m_width, 0, m_height );

	glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_texId );
	glBegin( GL_QUADS );
	{
		glTexCoord2i( x, y );					glVertex2d( x, y );
		glTexCoord2i( x + width, y );			glVertex2d( x + width, y );
		glTexCoord2i( x + width, y + height );	glVertex2d( x + width, y + height );
		glTexCoord2i( x, y + height );			glVertex2d( x, y + height );
	}
    glEnd();
	glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );

	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}

void drwDrawableTexture::Clear( int x, int y, int width, int height )
{
    glColor4d( 0.0, 0.0, 0.0, 0.0 );
    glBegin( GL_QUADS );
    {
        glVertex2d( x, y );
        glVertex2d( x + width, y );
        glVertex2d( x + width, y + height );
        glVertex2d( x, y + height );
    }
    glEnd();
}

void drwDrawableTexture::PasteToScreen()
{
    PasteToScreen( 0, 0, m_width, m_height );
}
