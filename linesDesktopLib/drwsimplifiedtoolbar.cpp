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
    connect( m_app, SIGNAL(EditParamsModified()), this, SLOT(UpdateUI()) );
    connect( m_app, SIGNAL(LineParamsModified()), this, SLOT(UpdateUI()) );
}

static double smallBrushWidth = 6.0;
static double smallBrushAlpha = 1.0;
static double bigBrushWidth = 100.0;
static double bigBrushAlpha = 0.5;
static int nbOnionOneBefore = 1;
static int nbOnionOneAfter = 1;
static int nbOnionManyBefore = 5;
static int nbOnionManyAfter = 5;

void drwSimplifiedToolbar::UpdateUI()
{
    if( !m_app )
        return;

    BlockSigs( true );

    // Line tool properties
    bool allOff = !m_app->IsSmallBrush() && !m_app->IsBigBrush() && !m_app->IsErasing();
    ui->paintToolButtonGroup->setExclusive( !allOff );
    ui->smallBrushButton->setChecked( m_app->IsSmallBrush() );
    ui->bigBrushButton->setChecked( m_app->IsBigBrush() );
    ui->eraserButton->setChecked( m_app->IsErasing() );

    // Frame change mode
    allOff = !m_app->IsFrameChangeManual() && !m_app->IsFrameChangeJumpAfter();
    ui->frameChangeButtonGroup->setExclusive( !allOff );
    ui->manualModeButton->setChecked( m_app->IsFrameChangeManual() );
    ui->jumpModeButton->setChecked( m_app->IsFrameChangeJumpAfter() );

    // Onion Skin
    allOff = !m_app->IsNoOnionSkin() && !m_app->IsOneOnionSkin() && !m_app->IsManyOnionSkin();
    ui->OnionSkinButtonGroup->setExclusive( !allOff );
    ui->noOnionButton->setChecked( m_app->IsNoOnionSkin() );
    ui->oneOnionButton->setChecked( m_app->IsOneOnionSkin() );
    ui->manyOnionButton->setChecked( m_app->IsManyOnionSkin() );

    BlockSigs( false );
}

void drwSimplifiedToolbar::BlockSigs( bool block )
{
    ui->smallBrushButton->blockSignals( block );
    ui->bigBrushButton->blockSignals( block );
    ui->eraserButton->blockSignals( block );
    ui->manualModeButton->blockSignals( block );
    ui->jumpModeButton->blockSignals( block );
    ui->noOnionButton->blockSignals( block );
    ui->oneOnionButton->blockSignals( block );
    ui->manyOnionButton->blockSignals( block );
}

void drwSimplifiedToolbar::on_smallBrushButton_toggled(bool checked)
{
    if( checked )
        m_app->SetSmallBrush();
}

void drwSimplifiedToolbar::on_bigBrushButton_toggled(bool checked)
{
    if( checked )
        m_app->SetBigBrush();
}

void drwSimplifiedToolbar::on_eraserButton_toggled(bool checked)
{
    if( checked )
        m_app->SetErasing();
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
    if( checked )
        m_app->SetOneOnionSkin();
}

void drwSimplifiedToolbar::on_manyOnionButton_toggled(bool checked)
{
    if( checked )
        m_app->SetManyOnionSkin();
}

void drwSimplifiedToolbar::on_noOnionButton_toggled(bool checked)
{
    if( checked )
        m_app->SetNoOnionSkin();
}
