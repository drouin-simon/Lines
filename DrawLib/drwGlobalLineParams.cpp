#include "drwGlobalLineParams.h"
#include "drwAutoGui.h"
#include "wideline.h"
#include "drwDrawingWidget.h"

drwGlobalLineParams::drwGlobalLineParams(QObject *parent) :
    QObject(parent)
{
    m_drawingWidget = 0;
}

QWidget * drwGlobalLineParams::CreateGui()
{
    drwAutoGui * ag = new drwAutoGui( this );
    ag->AddDoubleSliderParam("PixMargin", 0.5, 10.0 );
    ag->AddDoubleSliderParam("PixDampWidth", 0.1, 5.0 );
    ag->AddDoubleSliderParam("SigmaLarge", 0.1, 4.0 );
    ag->AddDoubleSliderParam("SigmaSmall", 0.1, 2.0 );
    connect( this, SIGNAL(Modified()), ag, SLOT(TargetModified()) );
    return ag;
}

void drwGlobalLineParams::SetPixMargin( double pixMarging )
{
    WideLine::SetPixMargin( (float) pixMarging );
    MarkModified();
}

double drwGlobalLineParams::GetPixMargin()
{
    return (double)WideLine::GetPixMargin();
}

void drwGlobalLineParams::SetPixDampWidth( double pixDampWidth )
{
    WideLine::SetPixDampWidth( (float) pixDampWidth );
    MarkModified();
}

double drwGlobalLineParams::GetPixDampWidth()
{
    return (double)WideLine::GetPixDampWidth();
}

void drwGlobalLineParams::SetSigmaLarge( double sigmaLarge )
{
    WideLine::SetSigmaLarge( (float) sigmaLarge );
    MarkModified();
}

double drwGlobalLineParams::GetSigmaLarge()
{
    return (double)WideLine::GetSigmaLarge();
}

void drwGlobalLineParams::SetSigmaSmall( double sigmaSmall )
{
    WideLine::SetSigmaSmall( (float) sigmaSmall );
    MarkModified();
}

double drwGlobalLineParams::GetSigmaSmall()
{
    return (double)WideLine::GetSigmaSmall();
}

void drwGlobalLineParams::MarkModified()
{
    Q_ASSERT( m_drawingWidget );
    m_drawingWidget->RequestRedraw();
    emit Modified();
}
