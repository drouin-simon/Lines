#include "ImageSprite.h"
#include <IncludeGl.h>
#include <QImage>
#include <QGLWidget>

const GLuint ImageSprite::InvalidTextureId = GLuint(-1);

ImageSprite::ImageSprite()
{
	ImageWidth = 0;
	ImageHeight = 0;
	TexCoordX = 1;
	TexCoordY = 1;
	TextureId = InvalidTextureId;
}


ImageSprite::~ImageSprite()
{
}


void ImageSprite::SetImage( QImage & image )
{
	ImageWidth = image.width();
	ImageHeight = image.height();
	
	QImage glImage = QGLWidget::convertToGLFormat( image );
	
	// Create OpenGl texture object
	glGenTextures( 1, &TextureId );
	glBindTexture(GL_TEXTURE_2D, TextureId );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	int nbComponents = 3;
	GLenum pixFormat = GL_RGB;
	if( glImage.depth() == 8 )
	{
		nbComponents = 1;
		pixFormat = GL_LUMINANCE;
	}
	else if( glImage.depth() == 16 )
	{
		nbComponents = 2;
		pixFormat = GL_LUMINANCE_ALPHA;
	}
	else if( glImage.depth() == 32 )
	{
		nbComponents = 4;
		pixFormat = GL_RGBA;
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, nbComponents, glImage.width(), glImage.height(), 
				 0, pixFormat, GL_UNSIGNED_BYTE, glImage.bits() );
}


void ImageSprite::InternDraw( const drwDrawingContext & context )
{
	if( TextureId != InvalidTextureId )
	{
		glBindTexture( GL_TEXTURE_2D, TextureId );
		glBegin( GL_QUADS );
			glTexCoord2d(0.0,0.0); glVertex2d(-.5,-.5);
			glTexCoord2d(TexCoordX,0.0); glVertex2d(double(ImageWidth)-.5,-.5);
			glTexCoord2d(TexCoordX,TexCoordY); glVertex2d(double(ImageWidth)-.5,double(ImageHeight)-.5);
			glTexCoord2d(0.0,TexCoordY); glVertex2d(-.5,double(ImageHeight)-.5);
		glEnd();
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
}

int ImageSprite::FindPowerOfTwo( int size )
{
	int newSize = 1;
	while( newSize < size )
		newSize *= 2;
	return newSize;
}
