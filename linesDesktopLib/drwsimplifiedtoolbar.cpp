#include "drwsimplifiedtoolbar.h"
#include "ui_drwsimplifiedtoolbar.h"
#include "LinesApp.h"

drwSimplifiedToolbar::drwSimplifiedToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::drwSimplifiedToolbar),
    m_app(0)
{
    ui->setupUi(this);
    UpdateUI();
}

drwSimplifiedToolbar::~drwSimplifiedToolbar()
{
    delete ui;
}

void drwSimplifiedToolbar::SetApp( LinesApp * app )
{
    m_app = app;
    connect( m_app, SIGNAL(DisplayParamsModified()), this, SLOT(UpdateUI()) );
    connect( m_app, SIGNAL(LineParamsModified()), this, SLOT(UpdateUI()) );
}

void drwSimplifiedToolbar::UpdateUI()
{
    if( !m_app )
        return;

    BlockSigs( true );

    // Line tool properties
    ui->smallBrushButton->setChecked( m_app->IsBrush() );
    ui->eraserButton->setChecked( m_app->IsErasing() );
    ui->opacityWidget->setSliderValue( m_app->GetLineColor()[3] );
    ui->pressureOpacityButton->setEnabled( m_app->IsPressureOpacityEnabled() );
    ui->pressureOpacityButton->setChecked( m_app->IsUsingPressureOpacity() );
    ui->pressureWidthButton->setEnabled( m_app->IsPressureWidthEnabled() );
    ui->pressureWidthButton->setChecked( m_app->IsUsingPressureWidth() );
    ui->fillButton->setChecked( m_app->IsFilling() );

    // Frame change mode
    ui->manualModeButton->setChecked( m_app->IsFrameChangeManual() );
    ui->jumpModeButton->setChecked( m_app->IsFrameChangeJumpAfter() );
    ui->playModeButton->setChecked( m_app->IsFrameChangePlay() );

    // Onion Skin
    ui->onionBeforeSpinBox->setValue( m_app->GetOnionSkinBefore() );
    ui->oneOnionButton->setChecked( m_app->IsOnionSkinEnabled() );
    ui->onionAfterSpinBox->setValue( m_app->GetOnionSkinAfter() );

    BlockSigs( false );
}

void drwSimplifiedToolbar::BlockSigs( bool block )
{
    ui->smallBrushButton->blockSignals( block );
    ui->eraserButton->blockSignals( block );
    ui->opacityWidget->blockSignals( block );
    ui->pressureWidthButton->blockSignals( block );
    ui->pressureOpacityButton->blockSignals( block );
    ui->fillButton->blockSignals( block );
    ui->manualModeButton->blockSignals( block );
    ui->jumpModeButton->blockSignals( block );
    ui->playModeButton->blockSignals( block );
    ui->oneOnionButton->blockSignals( block );
}

void drwSimplifiedToolbar::on_smallBrushButton_toggled(bool checked)
{
    if( checked )
        m_app->UseBrush();
}

void drwSimplifiedToolbar::on_eraserButton_toggled(bool checked)
{
    if( checked )
        m_app->SetErasing();
}

void drwSimplifiedToolbar::on_opacityWidget_sliderValueChanged( double val )
{
    Vec4 color( 1.0, 1.0, 1.0, val );
    m_app->SetLineColor( color );
}

void drwSimplifiedToolbar::on_manualModeButton_toggled(bool checked)
{
    if( checked )
        m_app->SetFrameChangeManual();
}

void drwSimplifiedToolbar::on_jumpModeButton_toggled(bool checked)
{
    if( checked )
        m_app->SetFrameChangeJumpAfter();
}

void drwSimplifiedToolbar::on_oneOnionButton_toggled(bool checked)
{
    m_app->SetOnionSkinEnabled( checked );
}

void drwSimplifiedToolbar::on_pressureWidthButton_toggled(bool checked)
{
    m_app->SetUsePressureWidth( checked );
}

void drwSimplifiedToolbar::on_pressureOpacityButton_toggled(bool checked)
{
    m_app->SetUsePressureOpacity( checked );
}

void drwSimplifiedToolbar::on_fillButton_toggled(bool checked)
{
    m_app->SetFill( checked );
}

void drwSimplifiedToolbar::on_onionBeforeSpinBox_valueChanged(int arg1)
{
    m_app->SetOnionSkinBefore( arg1 );
}

void drwSimplifiedToolbar::on_onionAfterSpinBox_valueChanged(int arg1)
{
    m_app->SetOnionSkinAfter( arg1 );
}

void drwSimplifiedToolbar::on_playModeButton_toggled(bool checked)
{
    if( checked )
        m_app->SetFrameChangePlay();
}
