#include "LinesApp.h"
#include "drwToolbox.h"
#include "drwLineTool.h"
#include "drwDrawingWidget.h"

static double smallBrushWidth = 6.0;
static double smallBrushAlpha = 1.0;
static double bigBrushWidth = 40.0;
static double bigBrushAlpha = 0.5;
static double eraserWidth = 40.0;
static double eraserAlpha = 1.0;
static double percentBrushIncrease = 0.2;
static int nbOnionOneBefore = 1;
static int nbOnionOneAfter = 0;
static int nbOnionManyBefore = 5;
static int nbOnionManyAfter = 5;

LinesApp::LinesApp( drwEditionState * edState, drwToolbox * toolbox )
    : m_backupBrushWidth( smallBrushWidth )
    , m_backupBrushOpacity( smallBrushAlpha )
    , m_editionState( edState )
    , m_localToolbox( toolbox )
    , m_drawingWidget( 0 )
{
    connect( m_editionState, SIGNAL(ModifiedSignal()), this, SLOT(EditParamsModifiedSlot()) );
    drwLineTool * lineTool = dynamic_cast<drwLineTool*>(m_localToolbox->GetTool( 0 ));
    Q_ASSERT(lineTool);
    connect( lineTool, SIGNAL(ParametersChangedSignal()), this, SLOT(LineParamsModifiedSlot()) );
}

LinesApp::~LinesApp()
{

}

void LinesApp::SetDrawingWidget( drwDrawingWidget * w )
{
    m_drawingWidget = w;
    connect( m_drawingWidget, SIGNAL(DisplaySettingsModified()), this, SLOT(DisplayParamsModifiedSlot()) );
}

bool LinesApp::IsSmallBrush()
{
    double w = GetLineWidth();
    double alpha = GetLineColor()[3];
    return ( w == smallBrushWidth && alpha == smallBrushAlpha && !IsErasing() );
}

void LinesApp::SetSmallBrush()
{
    SetLineWidth( smallBrushWidth );
    Vec4 col( 1.0, 1.0, 1.0, smallBrushAlpha );
    SetLineColor( col );
    GetLineTool()->SetErase( false );
    m_drawingWidget->SetCursorColor( "yellowgreen" );
}

bool LinesApp::IsBigBrush()
{
    double w = GetLineWidth();
    double alpha = GetLineColor()[3];
    bool isBig = ( w == bigBrushWidth && alpha == bigBrushAlpha && !IsErasing() );
    return isBig;
}

void LinesApp::SetBigBrush()
{
    SetLineWidth( bigBrushWidth );
    Vec4 col( 1.0, 1.0, 1.0, bigBrushAlpha );
    SetLineColor( col );
    GetLineTool()->SetErase( false );
    m_drawingWidget->SetCursorColor( "yellowgreen" );
}

void LinesApp::ToggleBigSmallBrush()
{
    if( IsSmallBrush() )
        SetBigBrush();
    else
        SetSmallBrush();
}

void LinesApp::IncreaseBrushSize()
{
    double size = GetLineWidth();
    SetLineWidth( size + percentBrushIncrease * size );
}

void LinesApp::DecreaseBrushSize()
{
    double size = GetLineWidth();
    SetLineWidth( size - percentBrushIncrease * size );
}

void LinesApp::SetLineWidth( double w )
{
    GetLineTool()->SetBaseWidth( w );
}

double LinesApp::GetLineWidth()
{
    return GetLineTool()->GetBaseWidth();
}

void LinesApp::SetLineColor( Vec4 & color )
{
    GetLineTool()->SetColor( color );
}

Vec4 LinesApp::GetLineColor()
{
    return GetLineTool()->GetColor();
}

void LinesApp::SetErasing()
{
    GetLineTool()->SetErase( true );
    SetLineWidth( eraserWidth );
    Vec4 color( 1.0, 1.0, 1.0, eraserAlpha );
    SetLineColor( color );
    m_drawingWidget->SetCursorColor( "orange" );
}

bool LinesApp::IsErasing()
{
    return GetLineTool()->GetErase();
}

void LinesApp::ToggleErasing()
{
    if( IsErasing() )
    {
        SetLineWidth( m_backupBrushWidth );
        Vec4 col( 1.0, 1.0, 1.0, m_backupBrushOpacity );
        SetLineColor( col );
        GetLineTool()->SetErase( false );
        m_drawingWidget->SetCursorColor( "yellowgreen" );
    }
    else
    {
        m_backupBrushWidth = GetLineWidth();
        m_backupBrushOpacity = GetLineColor()[3];
        SetErasing();
    }
}

bool LinesApp::IsFrameChangeManual()
{
    return GetFrameChangeMode() == Manual;
}

void LinesApp::SetFrameChangeManual()
{
    SetFrameChangeMode( Manual );
}

bool LinesApp::IsFrameChangeJumpAfter()
{
    return GetFrameChangeMode() == AfterIntervention;
}

void LinesApp::SetFrameChangeJumpAfter()
{
    SetFrameChangeMode( AfterIntervention );
}

drwFrameChangeMode LinesApp::GetFrameChangeMode()
{
    return m_editionState->GetFrameChangeMode();
}

void LinesApp::SetFrameChangeMode( drwFrameChangeMode mode )
{
    m_editionState->SetFrameChangeMode( mode );
}

bool LinesApp::IsNoOnionSkin() { return GetOnionSkinBefore() == 0 && GetOnionSkinAfter() == 0; }
void LinesApp::SetNoOnionSkin() { SetOnionSkinBefore(0); SetOnionSkinAfter(0); }
bool LinesApp::IsOneOnionSkin() { return GetOnionSkinBefore() == nbOnionOneBefore && GetOnionSkinAfter() == nbOnionOneAfter; }
void LinesApp::SetOneOnionSkin() { SetOnionSkinBefore( nbOnionOneBefore ); SetOnionSkinAfter( nbOnionOneAfter ); }
bool LinesApp::IsManyOnionSkin() { return GetOnionSkinBefore() == nbOnionManyBefore && GetOnionSkinAfter() == nbOnionManyAfter; }
void LinesApp::SetManyOnionSkin() { SetOnionSkinBefore( nbOnionManyBefore ); SetOnionSkinAfter( nbOnionManyAfter ); }

void LinesApp::SetOnionSkinBefore( int nbFrames ) { m_drawingWidget->SetOnionSkinBefore( nbFrames ); }
int LinesApp::GetOnionSkinBefore() { return m_drawingWidget->GetOnionSkinBefore(); }

void LinesApp::SetOnionSkinAfter( int nbFrames ) { m_drawingWidget->SetOnionSkinAfter( nbFrames ); }
int LinesApp::GetOnionSkinAfter() { return m_drawingWidget->GetOnionSkinAfter(); }

void LinesApp::LineParamsModifiedSlot()
{
    emit LineParamsModified();
}

void LinesApp::EditParamsModifiedSlot()
{
    emit EditParamsModified();
}

void LinesApp::DisplayParamsModifiedSlot()
{
    emit DisplayParamsModified();
}

drwLineTool * LinesApp::GetLineTool()
{
    drwLineTool * lineTool = dynamic_cast<drwLineTool*>(m_localToolbox->GetTool( 0 ));
    Q_ASSERT(lineTool);
    return lineTool;
}
