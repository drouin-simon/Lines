#include "LinesApp.h"
#include "LinesCore.h"
#include "drwNetworkManager.h"

static double percentBrushIncrease = 0.2;
static int nbOnionOneBefore = 1;
static int nbOnionOneAfter = 0;
static int nbOnionManyBefore = 5;
static int nbOnionManyAfter = 5;

LinesApp::LinesApp( LinesCore * lc, drwNetworkManager * netManager )
    : m_lines( lc )
    , m_netManager( netManager )
{
    drwLineTool * lineTool = GetLineTool();
    connect( lineTool, SIGNAL(ParametersChangedSignal()), this, SLOT(LineParamsModifiedSlot()) );
    connect( m_lines, SIGNAL(DisplaySettingsModified()), this, SLOT(DisplayParamsModifiedSlot()) );
    connect( m_netManager, SIGNAL(StateChangedSignal()), this, SLOT(NetworkManagerStateChangedSlot()) );
}

LinesApp::~LinesApp()
{
}

bool LinesApp::IsBrush()
{
    return !GetLineTool()->GetErase();
}

void LinesApp::UseBrush()
{
    GetLineTool()->SetErase( false );
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
}

bool LinesApp::IsErasing()
{
    return GetLineTool()->GetErase();
}

bool LinesApp::IsPressureWidthEnabled()
{
    return GetLineTool()->IsPresureWidthEnabled();
}

bool LinesApp::IsUsingPressureWidth()
{
    return GetLineTool()->GetPressureWidth();
}

void LinesApp::SetUsePressureWidth( bool use )
{
    GetLineTool()->SetPressureWidth( use );
}

bool LinesApp::IsPressureOpacityEnabled()
{
    return GetLineTool()->IsPresureOpacityEnabled();
}

bool LinesApp::IsUsingPressureOpacity()
{
    return GetLineTool()->GetPressureOpacity();
}

void LinesApp::SetUsePressureOpacity( bool use )
{
    GetLineTool()->SetPressureOpacity( use );
}

bool LinesApp::IsFilling()
{
    return GetLineTool()->GetFill();
}

void LinesApp::SetFill( bool fill )
{
    GetLineTool()->SetFill( fill );
}

void LinesApp::SetPersistence( int p )
{
    GetLineTool()->SetPersistence( p );
}

int LinesApp::GetPersistence()
{
    return GetLineTool()->GetPersistence();
}

void LinesApp::GotoNextFrame()
{
    m_lines->NextFrame();
}

void LinesApp::GotoPreviousFrame()
{
    m_lines->PrevFrame();
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

bool LinesApp::IsFrameChangePlay()
{
    return GetFrameChangeMode() == Play;
}

void LinesApp::SetFrameChangePlay()
{
    SetFrameChangeMode( Play );
}

drwFrameChangeMode LinesApp::GetFrameChangeMode()
{
    return GetLineTool()->GetFrameChangeMode();
}

void LinesApp::SetFrameChangeMode( drwFrameChangeMode mode )
{
    GetLineTool()->SetFrameChangeMode( mode );
}

void LinesApp::ToggleOnionSkinEnabled()
{
    SetOnionSkinEnabled( !IsOnionSkinEnabled() );
}

bool LinesApp::IsOnionSkinEnabled()
{
    return m_lines->GetOnionSkinEnabled();
}

void LinesApp::SetOnionSkinEnabled( bool enabled )
{
    m_lines->SetOnionSkinEnabled( enabled );
}

void LinesApp::SetOnionSkinBefore( int nbFrames ) { m_lines->SetOnionSkinBefore( nbFrames ); }
int LinesApp::GetOnionSkinBefore() { return m_lines->GetOnionSkinBefore(); }

void LinesApp::SetOnionSkinAfter( int nbFrames ) { m_lines->SetOnionSkinAfter( nbFrames ); }
int LinesApp::GetOnionSkinAfter() { return m_lines->GetOnionSkinAfter(); }

void LinesApp::ToggleInOnionSkin()
{
    m_lines->ToggleInOnionFrame();
}

void LinesApp::ToggleOutOnionSkin()
{
    m_lines->ToggleOutOnionFrame();
}

bool LinesApp::IsFlippingModeEnabled()
{
    return m_lines->IsFlippingFrameModeEnabled();
}

void LinesApp::SetFlippingModeEnabled( bool enabled )
{
    m_lines->SetFlippingFrameModeEnabled( enabled );
}

bool LinesApp::IsSharing()
{
    return m_netManager->IsSharing();
}

bool LinesApp::IsConnected()
{
    return m_netManager->IsConnected();
}

QString LinesApp::GetServerName()
{
    return m_netManager->GetServerUserName();
}

void LinesApp::LineParamsModifiedSlot()
{
    emit LineParamsModified();
}

void LinesApp::DisplayParamsModifiedSlot()
{
    emit DisplayParamsModified();
}

void LinesApp::NetworkManagerStateChangedSlot()
{
    emit NetworkManagerStateChangedSignal();
}

drwLineTool * LinesApp::GetLineTool()
{
    return m_lines->GetLineTool();
}
