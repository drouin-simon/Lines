#ifndef __drwDrawableTexture_h_
#define __drwDrawableTexture_h_

class drwDrawableTexture
{

public:

	drwDrawableTexture();
	~drwDrawableTexture();
	
	bool Init( int width, int height );
	void Resize( int width, int height );
	void Release();
	
	void DrawToTexture( bool drawTo );
    void PasteToScreen( int x, int y, int width, int height );
    void PasteToScreen();
    void Clear( int x, int y, int width, int height );

protected:
	
	unsigned m_texId;
	unsigned m_fbId;
	int m_width;
	int m_height;
};

#endif
