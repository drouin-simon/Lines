#ifndef __drwDrawingContext_h_
#define __drwDrawingContext_h_

#include "SVL.h"

class drwDrawingWidget;
class drwDrawableTexture;

class drwDrawingContext
{

public:

	drwDrawingContext( drwDrawingWidget * widget )
        : m_colorMultiplier( 1.0, 1.0, 1.0, 1.0 )
        , m_isPicking(false)
        , m_widget(widget) {}

    drwDrawableTexture * GetWorkingTexture() const;
    void WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin ) const;

    Vec4 m_colorMultiplier;
	bool m_isPicking;
	drwDrawingWidget * m_widget;
};


#endif
