#ifndef __drwDrawableTexture_h_
#define __drwDrawableTexture_h_

#include "../GraphicsEngine/GraphicsEngine.h"

class drwDrawableTexture
{

public:

	drwDrawableTexture();
	~drwDrawableTexture();
    
    void SetPixelFormatToRGBU8();
    void SetPixelFormatToRGBAF32();
    void SetPixelFormatToGreyF16();
    void SetPixelFormatToGreyF32();

    int GetWidth() { return m_width; }
    int GetHeight() { return m_height; }
	
	void Resize( int width, int height );
	void Release();
	
	void DrawToTexture( bool drawTo );
    void PasteToScreen( int x, int y, int width, int height );
    void PasteToScreen( int x, int y );
    void PasteToScreen();
    void Clear( int x, int y, int width, int height );

    void Upload( unsigned char * buffer );
    void Download( unsigned char * buffer );
    void Download( unsigned short * buffer );
    void Download( float * buffer );

    unsigned GetTexId() { return m_texId; }

protected:
	
    unsigned m_internalFormat;     // ex: GL_RGB16F
    unsigned m_pixelType;          // GL_RGB, GL_RGBA, GL_LUMINANCE, etc.
    unsigned m_componentType;      // GL_UNSIGNED_CHAR, GL_FLOAT, etc.
    unsigned m_downloadPixelType;  // Pixel type used when downloading texture, ex. GL_RED

    bool m_isDrawingInTexture;
	unsigned m_texId;
	unsigned m_fbId;
    int m_backupFbId;
	int m_width;
	int m_height;

    IGraphicsEngine* m_engine;
};

#endif
