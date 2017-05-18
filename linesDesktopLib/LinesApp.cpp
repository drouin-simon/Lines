#include "LinesApp.h"
#include "LinesCore.h"

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

LinesApp::LinesApp( LinesCore * lc )
    : m_backupBrushWidth( smallBrushWidth )
    , m_backupBrushOpacity( smallBrushAlpha )
    , m_lines( lc )
{
    drwLineTool * lineTool = GetLineTool();
    connect( lineTool, SIGNAL(ParametersChangedSignal()), this, SLOT(LineParamsModifiedSlot()) );
    connect( m_lines, SIGNAL(DisplaySettingsModified()), this, SLOT(DisplayParamsModifiedSlot()) );
}

LinesApp::~LinesApp()
{
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
    m_lines->SetLineBaseWidth( w );
}

double LinesApp::GetLineWidth()
{
    return GetLineTool()->GetBaseWidth();
}

void LinesApp::SetLineColor( Vec4 & color )
{
    m_lines->SetLineColor( color );
}

Vec4 LinesApp::GetLineColor()
{
    return GetLineTool()->GetColor();
}

void LinesApp::SetErasing()
{
    m_lines->SetLineErase( true );
    SetLineWidth( eraserWidth );
    Vec4 color( 1.0, 1.0, 1.0, eraserAlpha );
    SetLineColor( color );
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
    return GetLineTool()->GetFrameChangeMode();
}

void LinesApp::SetFrameChangeMode( drwFrameChangeMode mode )
{
    GetLineTool()->SetFrameChangeMode( mode );
}

bool LinesApp::IsNoOnionSkin() { return GetOnionSkinBefore() == 0 && GetOnionSkinAfter() == 0; }
void LinesApp::SetNoOnionSkin() { SetOnionSkinBefore(0); SetOnionSkinAfter(0); }
bool LinesApp::IsOneOnionSkin() { return GetOnionSkinBefore() == nbOnionOneBefore && GetOnionSkinAfter() == nbOnionOneAfter; }
void LinesApp::SetOneOnionSkin() { SetOnionSkinBefore( nbOnionOneBefore ); SetOnionSkinAfter( nbOnionOneAfter ); }
bool LinesApp::IsManyOnionSkin() { return GetOnionSkinBefore() == nbOnionManyBefore && GetOnionSkinAfter() == nbOnionManyAfter; }
void LinesApp::SetManyOnionSkin() { SetOnionSkinBefore( nbOnionManyBefore ); SetOnionSkinAfter( nbOnionManyAfter ); }

void LinesApp::SetOnionSkinBefore( int nbFrames ) { m_lines->SetOnionSkinBefore( nbFrames ); }
int LinesApp::GetOnionSkinBefore() { return m_lines->GetOnionSkinBefore(); }

void LinesApp::SetOnionSkinAfter( int nbFrames ) { m_lines->SetOnionSkinAfter( nbFrames ); }
int LinesApp::GetOnionSkinAfter() { return m_lines->GetOnionSkinAfter(); }

void LinesApp::LineParamsModifiedSlot()
{
    emit LineParamsModified();
}

void LinesApp::DisplayParamsModifiedSlot()
{
    emit DisplayParamsModified();
}

drwLineTool * LinesApp::GetLineTool()
{
    return m_lines->GetLineTool();
}
