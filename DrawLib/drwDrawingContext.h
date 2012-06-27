#ifndef __drwDrawingContext_h_
#define __drwDrawingContext_h_

#include "SVL.h"

class drwGLRenderer;
class drwDrawableTexture;

class drwDrawingContext
{

public:

    drwDrawingContext( drwGLRenderer * renderer )
        : m_colorMultiplier( 1.0, 1.0, 1.0, 1.0 )
        , m_isPicking(false)
        , m_renderer(renderer) {}

    drwDrawableTexture * GetWorkingTexture() const;
    void WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin ) const;
    double PixelsPerUnit() const;

    Vec4 m_colorMultiplier;
	bool m_isPicking;
    drwGLRenderer * m_renderer;
};


#endif
