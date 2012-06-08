#include "PrimitiveToolOptionWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include "drwEditionState.h"
#include "drwLineTool.h"
#include "drwGradientWidget.h"

PrimitiveToolOptionWidget::PrimitiveToolOptionWidget( drwEditionState * editionState, drwLineTool * lineTool, QWidget * parent )
: m_timerId(-1)
, m_updating(false)
, m_editionState(editionState)
, m_lineTool(lineTool)
{
	SetupUi();
	UpdateUi();
	
	connect( m_lineTool, SIGNAL(ParametersChangedSignal()), this, SLOT( ToolModified() ) );
	connect( m_editionState, SIGNAL(ModifiedSignal()), this, SLOT(ToolModified()) );
}

PrimitiveToolOptionWidget::~PrimitiveToolOptionWidget()
{
}

void PrimitiveToolOptionWidget::OnColorSliderValueChanged( double newSliderValue )
{
	if( !m_updating )
	{
		Vec4 newColor( newSliderValue, newSliderValue, newSliderValue, 1.0 );
		m_lineTool->SetColor( newColor );
	}
}

void PrimitiveToolOptionWidget::pressureWidthCheckBoxStateChanged( int state )
{
	if( !m_updating )
	{
		if( state == Qt::Unchecked )
			m_lineTool->SetPressureWidth( false );
		else if( state == Qt::Checked )
			m_lineTool->SetPressureWidth( true );
	}
}

void PrimitiveToolOptionWidget::pressureOpacityCheckBoxStateChanged( int state )
{
	if( !m_updating )
	{
		if( state == Qt::Unchecked )
			m_lineTool->SetPressureOpacity( false );
		else if( state == Qt::Checked )
			m_lineTool->SetPressureOpacity( true );
	}
}

void PrimitiveToolOptionWidget::fillCheckBoxStateChanged( int state )
{
	if( state == Qt::Unchecked )
		m_lineTool->SetFill( false );
	else if( state == Qt::Checked )
		m_lineTool->SetFill( true );
    UpdateUi();
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
        UpdateUi();
	}
}

void PrimitiveToolOptionWidget::OnPersistenceSpinBoxValueChanged( int value )
{
	if( !m_updating )
	{
		m_lineTool->SetPersistence( value );
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

	gradientWidget = new drwGradientWidget();
	QSizePolicy gradientPolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
	gradientWidget->setSizePolicy( gradientPolicy );
	gradientWidget->setMinimumHeight( 40 );
	connect( gradientWidget, SIGNAL(sliderValueChanged(double)), this, SLOT(OnColorSliderValueChanged(double) ) );
	mainLayout->addWidget( gradientWidget );
	mainLayout->addSpacing( 10 );

	{
		QGroupBox * pressureGroupBox = new QGroupBox(tr("Pressure"));
		QVBoxLayout * pressureLayout = new QVBoxLayout;
		pressureLayout->setSpacing( 15 );
		pressureGroupBox->setLayout( pressureLayout );
		pressureWidthCheckBox = new QCheckBox(tr("Width"));
		pressureLayout->addWidget( pressureWidthCheckBox );
		connect( pressureWidthCheckBox, SIGNAL(stateChanged(int)), this, SLOT(pressureWidthCheckBoxStateChanged(int)) );
		pressureOpacityCheckBox = new QCheckBox(tr("Opacity"));
		pressureLayout->addWidget( pressureOpacityCheckBox );
		connect( pressureOpacityCheckBox, SIGNAL(stateChanged(int)), this, SLOT(pressureOpacityCheckBoxStateChanged(int)) );
		mainLayout->addWidget( pressureGroupBox );
		mainLayout->addSpacing( 10 );
		
		fillCheckBox = new QCheckBox(tr("Fill lines"));
		connect( fillCheckBox, SIGNAL(stateChanged(int)), this, SLOT(fillCheckBoxStateChanged(int)) );
		mainLayout->addWidget( fillCheckBox );
		mainLayout->addSpacing( 10 );
		
		frameChangeModeGroupBox = new QGroupBox(tr("Frame Change"));
		frameChangeModeLayout = new QVBoxLayout;
		frameChangeModeLayout->setSpacing( 15 );
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
		
		mainLayout->addWidget( frameChangeModeGroupBox );
	}
	
	{
		persistenceLayout = new QHBoxLayout;
		persistenceLabel = new QLabel(tr("Persistence:"),this);
		persistenceLayout->addWidget(persistenceLabel);
		persistenceSpinBox = new QSpinBox(this);
        persistenceSpinBox->setFocusPolicy( Qt::NoFocus );  // so that it doesn't steal focus?
        persistenceSpinBox->setFrame( false );
        connect(persistenceSpinBox, SIGNAL(valueChanged(int)), this, SLOT(OnPersistenceSpinBoxValueChanged(int)) );
		persistenceLayout->addWidget(persistenceSpinBox);
	}
	mainLayout->addLayout(persistenceLayout);
	
}

void PrimitiveToolOptionWidget::UpdateUi()
{
	m_updating = true;

	Vec4 color = m_lineTool->GetColor();
	gradientWidget->setSliderValue( color[0] );
	
	if( m_editionState->GetFrameChangeMode() == Manual )
		manualFrameChangeRadio->setChecked( true );
	else if( m_editionState->GetFrameChangeMode() == AfterIntervention )
		jumpAfterFrameChangeRadio->setChecked( true );
	else if( m_editionState->GetFrameChangeMode() == Play )
		playFrameChangeRadio->setChecked( true );
	
	pressureWidthCheckBox->setChecked( m_lineTool->GetPressureWidth() );
    pressureWidthCheckBox->setEnabled( m_lineTool->IsPerssureWidthAndOpacityEnabled() );
	pressureOpacityCheckBox->setChecked( m_lineTool->GetPressureOpacity() );
    pressureOpacityCheckBox->setEnabled( m_lineTool->IsPerssureWidthAndOpacityEnabled() );
	fillCheckBox->setChecked( m_lineTool->GetFill() );
	persistenceSpinBox->setValue( m_lineTool->GetPersistence() );
    persistenceSpinBox->setEnabled( m_lineTool->IsPersistenceEnabled() );
	
	m_updating = false;
}
