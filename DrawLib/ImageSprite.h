#ifndef __ImageSprite_h_
#define __ImageSprite_h_

#include "IncludeGl.h"
#include "Primitive.h"

class drwSWFWriter;
class drwDrawingContext;
class Vec3;
class QImage;

class ImageSprite : public Primitive
{
	
public:
		
	ImageSprite();
	~ImageSprite();
	void SetImage( QImage & image );
		
protected:
	
    void InternDraw( drwDrawingContext & context );
	
	int FindPowerOfTwo( int size );
	
	int ImageWidth;
	int ImageHeight;
	double TexCoordX;
	double TexCoordY;
	GLuint TextureId;
	
	static const GLuint InvalidTextureId;
	
};


#endif
