#include "SideToolbar.h"
#include "ui_SideToolbar.h"
#include "LinesApp.h"

SideToolbar::SideToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SideToolbar)
{
    ui->setupUi(this);
    connect( ui->scrollWidget, SIGNAL(wheelClick(int)), this, SLOT(OnScrollWidgetClick(int) ) );
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

int SideToolbar::GetButtonWidth()
{
    return ui->nextFrameButton->minimumWidth();
}

void SideToolbar::SetButtonWidth( int w )
{
    ui->nextFrameButton->setMinimumSize( w, w );
    ui->prevFrameButton->setMinimumSize( w, w );
    ui->increaseSizeButton->setMinimumSize( w, w );
    ui->decreaseSizeButton->setMinimumSize( w, w );
}

void SideToolbar::OnScrollWidgetClick( int nbClicks )
{
    if( nbClicks > 0 )
        m_app->GotoPreviousFrame();
    else
        m_app->GotoNextFrame();
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
}
