#include "drwDrawableTexture.h"
#include "IncludeGLEW.h"
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
    m_engine = new OpenGLGraphicsEngine();
}

drwDrawableTexture::~drwDrawableTexture()
{
    Release();
    delete m_engine;
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
        m_texId = m_engine->InitTexture(0, m_internalFormat, width, height, 0, m_pixelType, m_componentType, 0);
        m_fbId = m_engine->InitFrameBuffer(m_texId);
    }

    if( m_width != width || m_height != height )
	{
        // Change texture size
        m_width = width;
		m_height = height;
        m_engine->DisplayTexture(m_texId, 0, m_internalFormat, width, height, 0, m_pixelType, m_componentType, 0);

        // Clear texture
        m_engine->ClearTexture(m_fbId);
	}
}

void drwDrawableTexture::Release()
{
    m_engine->ReleaseTexture(m_texId, m_fbId);
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
    m_engine->PasteTextureToScreen(m_texId, m_width, m_height, x, y, width, height);
}

void drwDrawableTexture::Clear( int x, int y, int width, int height )
{
    m_engine->ClearScreen(m_width, m_height, x, y, width, height);
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
    m_engine->Upload(m_texId, 0, m_internalFormat, m_width, m_height, 0, m_pixelType, GL_UNSIGNED_BYTE, buffer);
}

void drwDrawableTexture::Download( unsigned char * buffer )
{
    m_engine->Download(m_texId, 0, m_downloadPixelType, GL_UNSIGNED_BYTE, buffer);
}

void drwDrawableTexture::Download( unsigned short * buffer )
{
    m_engine->Download(m_texId, 0, m_downloadPixelType, GL_UNSIGNED_SHORT, buffer);
}

void drwDrawableTexture::Download( float * buffer )
{
    m_engine->Download(m_texId, 0, GL_RED, GL_FLOAT, buffer);
}
