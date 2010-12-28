#include "PrimitiveToolOptionWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include "drwEditionState.h"

PrimitiveToolOptionWidget::PrimitiveToolOptionWidget( drwEditionState * editionState, QWidget * parent )
: m_timerId(-1)
, m_updating(false)
, m_editionState(editionState)
{
	SetupUi();
	UpdateUi();
	
	connect( m_editionState, SIGNAL(ModifiedSignal()), this, SLOT(ToolModified()) );
}

PrimitiveToolOptionWidget::~PrimitiveToolOptionWidget()
{
}

void PrimitiveToolOptionWidget::FrameChangeRadioToggled( bool isOn )
{
	if( isOn && !m_updating )
	{
		// find which button is on
		if( manualFrameChangeRadio->isChecked() )
			m_editionState->SetFrameChangeMode( Manual );
		else if( jumpAfterFrameChangeRadio->isChecked() )
			m_editionState->SetFrameChangeMode( AfterIntervention );
		else 
			m_editionState->SetFrameChangeMode( Play );
	}
}

void PrimitiveToolOptionWidget::OnPersistenceSpinBoxValueChanged( int value )
{
	if( !m_updating )
	{
		m_editionState->SetPersistence( value );
	}
}

void PrimitiveToolOptionWidget::ToolModified()
{
	if( m_timerId == -1 )
		m_timerId = startTimer(0);
}

void PrimitiveToolOptionWidget::timerEvent(QTimerEvent *event)
{
	killTimer( m_timerId );
	m_timerId = -1;
	UpdateUi();
}

void PrimitiveToolOptionWidget::SetupUi()
{
	QSizePolicy mainPolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
	setSizePolicy( mainPolicy );
	
	mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins( 9, 0, 9, 0 );
	setLayout( mainLayout );
	{
		frameChangeModeGroupBox = new QGroupBox(tr("Frame change mode"));
		frameChangeModeLayout = new QVBoxLayout;
		frameChangeModeGroupBox->setLayout( frameChangeModeLayout );
		manualFrameChangeRadio = new QRadioButton(tr("Manual"),frameChangeModeGroupBox);
		frameChangeModeLayout->addWidget( manualFrameChangeRadio );
		connect( manualFrameChangeRadio, SIGNAL(toggled(bool)), this, SLOT(FrameChangeRadioToggled(bool)) );
		jumpAfterFrameChangeRadio = new QRadioButton(tr("Jump after"),frameChangeModeGroupBox);
		frameChangeModeLayout->addWidget( jumpAfterFrameChangeRadio );
		connect( jumpAfterFrameChangeRadio, SIGNAL(toggled(bool)), this, SLOT(FrameChangeRadioToggled(bool)) );
		playFrameChangeRadio = new QRadioButton(tr("Play"),frameChangeModeGroupBox);
		frameChangeModeLayout->addWidget( playFrameChangeRadio );
		connect( playFrameChangeRadio, SIGNAL(toggled(bool)), this, SLOT(FrameChangeRadioToggled(bool)) );
	}
	mainLayout->addWidget( frameChangeModeGroupBox );
	
	{
		persistenceLayout = new QHBoxLayout;
		persistenceLabel = new QLabel(tr("Persistence:"),this);
		persistenceLayout->addWidget(persistenceLabel);
		persistenceSpinBox = new QSpinBox(this);
		connect(persistenceSpinBox, SIGNAL(valueChanged(int)), this, SLOT(OnPersistenceSpinBoxValueChanged(int)) );
		persistenceLayout->addWidget(persistenceSpinBox);
	}
	mainLayout->addLayout(persistenceLayout);
	
}

void PrimitiveToolOptionWidget::UpdateUi()
{
	m_updating = true;
	
	if( m_editionState->GetFrameChangeMode() == Manual )
		manualFrameChangeRadio->setChecked( true );
	else if( m_editionState->GetFrameChangeMode() == AfterIntervention )
		jumpAfterFrameChangeRadio->setChecked( true );
	else if( m_editionState->GetFrameChangeMode() == Play )
		playFrameChangeRadio->setChecked( true );
	
	persistenceSpinBox->setValue( m_editionState->GetPersistence() );
	
	m_updating = false;
}
