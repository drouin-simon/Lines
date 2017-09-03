#include "LinesPreferencesWidget.h"
#include "ui_LinesPreferencesWidget.h"
#include "MainWindow.h"

LinesPreferencesWidget::LinesPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LinesPreferencesWidget)
{
    m_mainWindow = 0;
    ui->setupUi(this);
}

LinesPreferencesWidget::~LinesPreferencesWidget()
{
    delete ui;
}

void LinesPreferencesWidget::SetMainWindow( MainWindow * mw )
{
    m_mainWindow = mw;
    UpdateUI();
}

void LinesPreferencesWidget::UpdateUI()
{
    Q_ASSERT( m_mainWindow );

    ui->sideToolbarGroupBox->blockSignals( true );
    ui->sideToolbarGroupBox->setChecked( m_mainWindow->IsSideToolbarShown() );
    ui->sideToolbarGroupBox->blockSignals( false );

    ui->sideToolbarLeftRadioButton->blockSignals( true );
    ui->sideToolbarRightRadioButton->blockSignals( true );
    ui->sideToolbarLeftRadioButton->setChecked( m_mainWindow->IsSideToolbarLeft() );
    ui->sideToolbarLeftRadioButton->blockSignals( false );
    ui->sideToolbarRightRadioButton->blockSignals( false );

    ui->mainToolbarHeightSpinBox->blockSignals( true );
    ui->mainToolbarHeightSpinBox->setValue( m_mainWindow->GetMainToolbarHeight() );
    ui->mainToolbarHeightSpinBox->blockSignals( false );

    ui->sideToolbarWidthSpinBox->blockSignals( true );
    ui->sideToolbarWidthSpinBox->setValue( m_mainWindow->GetSideToolbarWidth() );
    ui->sideToolbarWidthSpinBox->blockSignals( false );
}

void LinesPreferencesWidget::on_sideToolbarGroupBox_toggled(bool arg1)
{
    m_mainWindow->SetShowSideToolbar( arg1 );
}

void LinesPreferencesWidget::on_sideToolbarLeftRadioButton_toggled(bool checked)
{
    if( checked )
        m_mainWindow->SetSideToolbarLeft( true );
}

void LinesPreferencesWidget::on_sideToolbarRightRadioButton_toggled(bool checked)
{
    if( checked )
        m_mainWindow->SetSideToolbarLeft( false );
}

void LinesPreferencesWidget::on_sideToolbarWidthSpinBox_valueChanged(int arg1)
{
    m_mainWindow->SetSideToolbarWidth( arg1 );
}

void LinesPreferencesWidget::on_mainToolbarHeightSpinBox_valueChanged(int arg1)
{
    m_mainWindow->SetMainToolbarHeight( arg1 );
}
