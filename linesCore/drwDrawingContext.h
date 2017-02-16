#ifndef __drwDrawingContext_h_
#define __drwDrawingContext_h_

#include "SVL.h"

class drwGLRenderer;
class drwDrawableTexture;
class drwGlslShader;

class drwDrawingContext
{

public:

    drwDrawingContext( drwGLRenderer * renderer, Box2d & rect )
        : m_colorMultiplier( 1.0, 1.0, 1.0, 1.0 )
        , m_isPicking(false)
        , m_drawingRect( rect )
        , m_renderer(renderer) {}

    drwDrawableTexture * GetWorkingTexture() const;
    drwGlslShader * GetWidelineShader() const;
    void SetWidelineShader( drwGlslShader * shader );
    void WorldToGLFrame( double xworld, double yworld, int & xwin, int & ywin ) const;
    double PixelsPerUnit() const;
    Box2d & DrawingRect() { return m_drawingRect; }

    Vec4 m_colorMultiplier;
	bool m_isPicking;
    Box2d m_drawingRect;
    drwGLRenderer * m_renderer;
};


#endif
