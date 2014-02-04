#include "drwDrawableTexture.h"
#include "IncludeGLee.h"
#include "IncludeGl.h"
#include <assert.h>

drwDrawableTexture::drwDrawableTexture()
    : m_internalFormat( GL_RGBA )
    , m_pixelType( GL_RGBA )
    , m_componentType( GL_UNSIGNED_BYTE )
    , m_downloadPixelType( GL_RGBA )
    , m_isDrawingInTexture( false )
    , m_texId(0)
	, m_fbId(0)
    , m_backupFbId(0)
	, m_width(1)
	, m_height(1)
{
}

drwDrawableTexture::~drwDrawableTexture()
{
	Release();
}

void drwDrawableTexture::SetPixelFormatToRGBU8() { m_internalFormat = GL_RGB; m_pixelType = GL_RGB; m_componentType = GL_UNSIGNED_BYTE; m_downloadPixelType = GL_RGB; }
//void drwDrawableTexture::SetPixelFormatToGreyF16() { m_internalFormat = GL_R16F; m_pixelType = GL_LUMINANCE; m_componentType = GL_FLOAT; m_downloadPixelType = GL_RED; }
void drwDrawableTexture::SetPixelFormatToGreyF32() { m_internalFormat = GL_R32F; m_pixelType = GL_RED; m_componentType = GL_FLOAT; m_downloadPixelType = GL_RED; }
void drwDrawableTexture::SetPixelFormatToRGBAF32() { m_internalFormat = GL_RGBA32F_ARB; m_pixelType = GL_RGBA; m_componentType = GL_FLOAT; m_downloadPixelType = GL_RGBA; }

void drwDrawableTexture::Resize( int width, int height )
{
    // Init texture and fbo if needed
    if( !m_texId )
    {
        glGenTextures( 1, &m_texId );
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_texId );
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, m_internalFormat, width, height, 0, m_pixelType, m_componentType, 0 );
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0 );

        int backupFbId = 0;
        glGetIntegerv( GL_FRAMEBUFFER_BINDING, &backupFbId );
        glGenFramebuffersEXT( 1, &m_fbId );
        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_fbId );
        glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, m_texId, 0 );
        GLenum ret = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
        assert( ret == GL_FRAMEBUFFER_COMPLETE_EXT );
        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, backupFbId );
    }

    if( m_width != width || m_height != height )
	{
        // Change texture size
        m_width = width;
		m_height = height;
		glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_texId );
        glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, m_internalFormat, width, height, 0, m_pixelType, m_componentType, 0 );
		glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );

        // Clear texture
        int backupFbId = 0;
        glGetIntegerv( GL_FRAMEBUFFER_BINDING, &backupFbId );
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_fbId );
		glClearColor( 0.0, 0.0, 0.0, 0.0 );
		glClear( GL_COLOR_BUFFER_BIT );
        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, backupFbId );
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
    assert( m_fbId );
    if( drawTo )
    {
        assert( !m_isDrawingInTexture );
        glGetIntegerv( GL_FRAMEBUFFER_BINDING, &m_backupFbId );
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_fbId );
        m_isDrawingInTexture = true;
    }
	else
    {
        assert( m_isDrawingInTexture );
        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_backupFbId );
        m_isDrawingInTexture = false;
    }
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
    
    glColor4d( 1.0, 1.0, 1.0, 1.0 );

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
    glDisable( GL_BLEND );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0, m_width, 0, m_height );

    glColor4d( 0.0, 0.0, 0.0, 0.0 );
    glBegin( GL_QUADS );
    {
        glVertex2d( x, y );
        glVertex2d( x + width, y );
        glVertex2d( x + width, y + height );
        glVertex2d( x, y + height );
    }
    glEnd();

    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glEnable( GL_BLEND );
}

void drwDrawableTexture::PasteToScreen( int x, int y )
{
    PasteToScreen( x, y, m_width, m_height );
}

void drwDrawableTexture::PasteToScreen()
{
    PasteToScreen( 0, 0, m_width, m_height );
}

void drwDrawableTexture::Upload( unsigned char * buffer )
{
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_texId );
    glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, m_internalFormat, m_width, m_height, 0, m_pixelType, GL_UNSIGNED_BYTE, buffer );
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
}

void drwDrawableTexture::Download( unsigned char * buffer )
{
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_texId );
    glGetTexImage( GL_TEXTURE_RECTANGLE_ARB, 0, m_downloadPixelType, GL_UNSIGNED_BYTE, buffer );
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
}

void drwDrawableTexture::Download( unsigned short * buffer )
{
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_texId );
    glGetTexImage( GL_TEXTURE_RECTANGLE_ARB, 0, m_downloadPixelType, GL_UNSIGNED_SHORT, buffer );
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
}

void drwDrawableTexture::Download( float * buffer )
{
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_texId );
    glGetTexImage( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RED, GL_FLOAT, buffer );
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
}
