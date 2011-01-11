#ifndef __drwDrawingContext_h_
#define __drwDrawingContext_h_

#include "SVL.h"

class drwDrawingWidget;
class drwDrawableTexture;

class drwDrawingContext
{

public:

	drwDrawingContext( drwDrawingWidget * widget )
        : m_opacity(1.0)
        , m_isOverridingColor(false)
        , m_colorOverride(1.0,1.0,1.0)
        , m_isPicking(false)
        , m_widget(widget) {}

    drwDrawableTexture * GetWorkingTexture() const;
    void WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin ) const;

	double m_opacity;
	bool m_isOverridingColor;
	Vec3 m_colorOverride;
	bool m_isPicking;
	drwDrawingWidget * m_widget;
};


#endif
