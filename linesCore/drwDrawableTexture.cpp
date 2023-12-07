#include "drwDrawableTexture.h"
#include <assert.h>

drwDrawableTexture::drwDrawableTexture()
    : m_internalFormat( GraphicsEngine::TYPES_MAPPING::RGBA )
    , m_pixelType(GraphicsEngine::TYPES_MAPPING::RGBA)
    , m_componentType(GraphicsEngine::TYPES_MAPPING::UBYTE)
    , m_downloadPixelType(GraphicsEngine::TYPES_MAPPING::RGBA)
    , m_isDrawingInTexture( false )
    , m_texId(0)
	, m_fbId(0)
    , m_backupFbId(0)
	, m_width(1)
	, m_height(1)
{
    m_engine = new GraphicsEngine();
}

drwDrawableTexture::~drwDrawableTexture()
{
    Release();
    delete m_engine;
}

void drwDrawableTexture::SetPixelFormatToRGBU8() { m_internalFormat = GraphicsEngine::TYPES_MAPPING::RGB; m_pixelType = GraphicsEngine::TYPES_MAPPING::RGB; m_componentType = GraphicsEngine::TYPES_MAPPING::UBYTE; m_downloadPixelType = GraphicsEngine::TYPES_MAPPING::RGB; }
//void drwDrawableTexture::SetPixelFormatToGreyF16() { m_internalFormat = GL_R16F; m_pixelType = GL_LUMINANCE; m_componentType = GL_FLOAT; m_downloadPixelType = GL_RED; }
void drwDrawableTexture::SetPixelFormatToGreyF32() { m_internalFormat = GraphicsEngine::TYPES_MAPPING::R32F; m_pixelType = GraphicsEngine::TYPES_MAPPING::RED; m_componentType = GraphicsEngine::TYPES_MAPPING::F32; m_downloadPixelType = GraphicsEngine::TYPES_MAPPING::RED; }
void drwDrawableTexture::SetPixelFormatToRGBAF32() { m_internalFormat = GraphicsEngine::TYPES_MAPPING::RGBA /*GL_RGBA32F_ARB*/; m_pixelType = GraphicsEngine::TYPES_MAPPING::RGBA; m_componentType = GraphicsEngine::TYPES_MAPPING::F32; m_downloadPixelType = GraphicsEngine::TYPES_MAPPING::RGBA; }

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
        m_engine->DrawToTexture(m_fbId, m_backupFbId);
        m_isDrawingInTexture = true;
    }
	else
    {
        assert( m_isDrawingInTexture );
        m_engine->BindFrameBuffer(m_backupFbId);
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
    m_engine->UploadUnsignedByte(m_texId, 0, m_internalFormat, m_width, m_height, 0, m_pixelType, buffer);
}

void drwDrawableTexture::Download( unsigned char * buffer )
{
    m_engine->DownloadUnsignedByte(m_texId, 0, m_downloadPixelType, buffer);
}

void drwDrawableTexture::Download( unsigned short * buffer )
{
    m_engine->DownloadUnsignedShort(m_texId, 0, m_downloadPixelType, buffer);
}

void drwDrawableTexture::Download( float * buffer )
{
    m_engine->DownloadFloat(m_texId, 0, GraphicsEngine::TYPES_MAPPING::RED, buffer);
}
