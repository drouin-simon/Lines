#ifndef __drwDrawableTexture_h_
#define __drwDrawableTexture_h_

class drwDrawableTexture
{

public:

	drwDrawableTexture();
	~drwDrawableTexture();
	
	void Resize( int width, int height );
	void Release();
	
	void DrawToTexture( bool drawTo );
    void PasteToScreen( int x, int y, int width, int height );
    void PasteToScreen();
    void Clear( int x, int y, int width, int height );

protected:
	
    bool m_isDrawingInTexture;
	unsigned m_texId;
	unsigned m_fbId;
    int m_backupFbId;
	int m_width;
	int m_height;
};

#endif
