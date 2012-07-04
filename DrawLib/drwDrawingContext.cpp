#include "drwDrawingContext.h"
#include "drwGLRenderer.h"

drwDrawableTexture * drwDrawingContext::GetWorkingTexture() const
{
    return m_renderer->GetWorkTexture();
}

drwGlslShader * drwDrawingContext::GetWidelineShader() const
{
    return m_renderer->GetWidelineShader();
}

void drwDrawingContext::SetWidelineShader( drwGlslShader * shader )
{
    m_renderer->SetWidelineShader( shader );
}

void drwDrawingContext::WorldToGLWindow( double xworld, double yworld, int & xwin, int & ywin ) const
{
    m_renderer->WorldToGLWindow( xworld, yworld, xwin, ywin );
}

double drwDrawingContext::PixelsPerUnit() const
{
    return m_renderer->PixelsPerUnit();
}
