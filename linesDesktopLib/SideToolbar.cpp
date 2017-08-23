#include "SideToolbar.h"
#include "ui_SideToolbar.h"
#include "LinesApp.h"

SideToolbar::SideToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SideToolbar)
{
    ui->setupUi(this);
}

SideToolbar::~SideToolbar()
{
    delete ui;
}

void SideToolbar::SetApp( LinesApp * app )
{
    m_app = app;
    UpdateUi();
}

void SideToolbar::on_flipModeButton_toggled(bool checked)
{
    m_app->SetFlippingModeEnabled( checked );
    UpdateUi();
}

void SideToolbar::on_nextFrameButton_clicked()
{
    m_app->GotoNextFrame();
}

void SideToolbar::on_prevFrameButton_clicked()
{
    m_app->GotoPreviousFrame();
}

void SideToolbar::on_increaseSizeButton_clicked()
{
    m_app->IncreaseBrushSize();
}

void SideToolbar::on_decreaseSizeButton_clicked()
{
    m_app->DecreaseBrushSize();
}

void SideToolbar::UpdateUi()
{
    ui->flipModeButton->blockSignals( true );
    ui->flipModeButton->setChecked( m_app->IsFlippingModeEnabled() );
    ui->flipModeButton->blockSignals( false );
}
