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
, m_timerId(-1)
, m_updating(false)
, m_displaySettings(displaySettings)
{
	SetupUi();
	UpdateUi();
}

DisplaySettingsWidget::~DisplaySettingsWidget()
{
}

void DisplaySettingsWidget::OnFramesBeforeValueChanged( int value )
{
	if(!m_updating)
		m_displaySettings->SetOnionSkinBefore( value );
}

void DisplaySettingsWidget::OnFramesAfterValueChanged( int value )
{
	if(!m_updating)
		m_displaySettings->SetOnionSkinAfter( value );
}

void DisplaySettingsWidget::OnDisplayCameraFrameChecked( bool isOn )
{
	if( !m_updating )
		m_displaySettings->SetShowCameraFrame( isOn );
}

void DisplaySettingsWidget::SettingsModified()
{
	if( m_timerId != -1 )
		m_timerId = startTimer( 0 );
}

void DisplaySettingsWidget::timerEvent(QTimerEvent *event)
{
	killTimer( m_timerId );
	m_timerId = -1;
	UpdateUi();
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

void DisplaySettingsWidget::UpdateUi()
{
	m_updating = true;
	
	framesBeforeSpinBox->setValue( m_displaySettings->GetOnionSkinBefore() );
	framesAfterSpinBox->setValue( m_displaySettings->GetOnionSkinAfter() );
	displayCameraFrameCheckBox->setChecked( m_displaySettings->GetShowCameraFrame() );
		
	m_updating = false;
}
