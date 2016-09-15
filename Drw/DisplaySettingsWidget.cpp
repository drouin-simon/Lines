#include "DisplaySettingsWidget.h"
#include "drwDisplaySettings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QGroupBox>


DisplaySettingsWidget::DisplaySettingsWidget( drwDisplaySettings * displaySettings, QWidget * parent )
: QWidget( parent )
, m_displaySettings(displaySettings)
{
	SetupUi();
    connect( m_displaySettings, SIGNAL(ModifiedSignal()), this, SLOT(UpdateUI()) );
    UpdateUI();
}

DisplaySettingsWidget::~DisplaySettingsWidget()
{
}

void DisplaySettingsWidget::OnFramesBeforeValueChanged( int value )
{
    m_displaySettings->SetOnionSkinBefore( value );
}

void DisplaySettingsWidget::OnFramesAfterValueChanged( int value )
{
    m_displaySettings->SetOnionSkinAfter( value );
}

void DisplaySettingsWidget::OnDisplayCameraFrameChecked( bool isOn )
{
    m_displaySettings->SetShowCameraFrame( isOn );
}

void DisplaySettingsWidget::UpdateUI()
{
    BlockSigs( true );
    framesBeforeSpinBox->setValue( m_displaySettings->GetOnionSkinBefore() );
    framesAfterSpinBox->setValue( m_displaySettings->GetOnionSkinAfter() );
    displayCameraFrameCheckBox->setChecked( m_displaySettings->GetShowCameraFrame() );
    BlockSigs( false );
}

void DisplaySettingsWidget::SetupUi()
{
	QSizePolicy mainPolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
	setSizePolicy( mainPolicy );
	
	mainLayout = new QVBoxLayout;
	mainLayout->setContentsMargins( 9, 0, 9, 0 );
	setLayout( mainLayout );
	
	{
		onionSkinGroupBox = new QGroupBox(tr("Onion skin"), this);
		onionSkinLayout = new QVBoxLayout;
		onionSkinGroupBox->setLayout( onionSkinLayout );
		{
			framesBeforeLayout = new QHBoxLayout;
			framesBeforeLabel = new QLabel(tr("Before:"),onionSkinGroupBox);
			framesBeforeLayout->addWidget( framesBeforeLabel );
			framesBeforeSpinBox = new QSpinBox(this);
            framesBeforeSpinBox->setFocusPolicy( Qt::NoFocus );
            framesBeforeSpinBox->setFrame( false );
			framesBeforeLayout->addWidget( framesBeforeSpinBox );
			connect( framesBeforeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(OnFramesBeforeValueChanged(int)) );
		}
		onionSkinLayout->addLayout( framesBeforeLayout );
		{
			framesAfterLayout = new QHBoxLayout;
			framesAfterLabel = new QLabel(tr("After:"),onionSkinGroupBox);
			framesAfterLayout->addWidget( framesAfterLabel );
			framesAfterSpinBox = new QSpinBox(onionSkinGroupBox);
            framesAfterSpinBox->setFocusPolicy( Qt::NoFocus );
            framesAfterSpinBox->setFrame( false );
			framesAfterLayout->addWidget( framesAfterSpinBox );
			connect( framesAfterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(OnFramesAfterValueChanged(int)) );
		}
		onionSkinLayout->addLayout( framesAfterLayout );
	}
	mainLayout->addWidget(onionSkinGroupBox);
	
	displayCameraFrameCheckBox = new QCheckBox(tr("Display Camera frame"));
	connect( displayCameraFrameCheckBox, SIGNAL(toggled(bool)), this, SLOT(OnDisplayCameraFrameChecked( bool )) );
	mainLayout->addWidget( displayCameraFrameCheckBox );	
}

void DisplaySettingsWidget::BlockSigs( bool block )
{
    framesBeforeSpinBox->blockSignals( block );
    framesAfterSpinBox->blockSignals( block );
    displayCameraFrameCheckBox->blockSignals( block );
}
