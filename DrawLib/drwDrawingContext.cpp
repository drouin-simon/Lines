#include "drwDrawingContext.h"
#include "drwDrawingWidget.h"

drwDrawableTexture * drwDrawingContext::GetWorkingTexture() const
{
    return m_widget->GetWorkTexture();
}

void drwDrawingContext::WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin ) const
{
    m_widget->WorldToGLWindow( xworld, yworld, xwin, ywin );
}
