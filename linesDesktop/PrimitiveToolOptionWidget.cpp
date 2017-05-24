#include "PrimitiveToolOptionWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include "drwLineTool.h"
#include "drwGradientWidget.h"

PrimitiveToolOptionWidget::PrimitiveToolOptionWidget( drwLineTool * lineTool, QWidget * parent )
: m_lineTool(lineTool)
{
	SetupUi();
	UpdateUi();
	connect( m_lineTool, SIGNAL(ParametersChangedSignal()), this, SLOT( ToolModified() ) );
}

PrimitiveToolOptionWidget::~PrimitiveToolOptionWidget()
{
}

void PrimitiveToolOptionWidget::OnColorSliderValueChanged( double newSliderValue )
{
    Vec4 newColor( newSliderValue, newSliderValue, newSliderValue, 1.0 );
    m_lineTool->SetColor( newColor );
}

void PrimitiveToolOptionWidget::pressureWidthCheckBoxStateChanged( int state )
{
    if( state == Qt::Unchecked )
        m_lineTool->SetPressureWidth( false );
    else if( state == Qt::Checked )
        m_lineTool->SetPressureWidth( true );
}

void PrimitiveToolOptionWidget::pressureOpacityCheckBoxStateChanged( int state )
{
    if( state == Qt::Unchecked )
        m_lineTool->SetPressureOpacity( false );
    else if( state == Qt::Checked )
        m_lineTool->SetPressureOpacity( true );
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
    if( isOn )
	{
		// find which button is on
		if( manualFrameChangeRadio->isChecked() )
            m_lineTool->SetFrameChangeMode( Manual );
		else if( jumpAfterFrameChangeRadio->isChecked() )
            m_lineTool->SetFrameChangeMode( AfterIntervention );
		else 
            m_lineTool->SetFrameChangeMode( Play );
        UpdateUi();
	}
}

void PrimitiveToolOptionWidget::OnPersistenceSpinBoxValueChanged( int value )
{
    m_lineTool->SetPersistence( value );
}

void PrimitiveToolOptionWidget::ToolModified()
{
    UpdateUi();
}

void PrimitiveToolOptionWidget::SetupUi()
{
	QSizePolicy mainPolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
	setSizePolicy( mainPolicy );
	
	mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins( 9, 0, 9, 0 );
	setLayout( mainLayout );

    //gradientWidget = new drwGradientWidget();
    //QSizePolicy gradientPolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
    //gradientWidget->setSizePolicy( gradientPolicy );
    //gradientWidget->setMinimumHeight( 40 );
    //connect( gradientWidget, SIGNAL(sliderValueChanged(double)), this, SLOT(OnColorSliderValueChanged(double) ) );
    //mainLayout->addWidget( gradientWidget );
    mainLayout->addSpacing( 15 );

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
    manualFrameChangeRadio->blockSignals( true );
    jumpAfterFrameChangeRadio->blockSignals( true );
    playFrameChangeRadio->blockSignals( true );

    if( m_lineTool->GetFrameChangeMode() == Manual )
		manualFrameChangeRadio->setChecked( true );
    else if( m_lineTool->GetFrameChangeMode() == AfterIntervention )
		jumpAfterFrameChangeRadio->setChecked( true );
    else if( m_lineTool->GetFrameChangeMode() == Play )
		playFrameChangeRadio->setChecked( true );

    manualFrameChangeRadio->blockSignals( false );
    jumpAfterFrameChangeRadio->blockSignals( false );
    playFrameChangeRadio->blockSignals( false );
	
    pressureWidthCheckBox->blockSignals( true );
	pressureWidthCheckBox->setChecked( m_lineTool->GetPressureWidth() );
    pressureWidthCheckBox->setEnabled( m_lineTool->IsPresureWidthEnabled() );
    pressureWidthCheckBox->blockSignals( false );

    pressureOpacityCheckBox->blockSignals( true );
	pressureOpacityCheckBox->setChecked( m_lineTool->GetPressureOpacity() );
    pressureOpacityCheckBox->setEnabled( m_lineTool->IsPresureOpacityEnabled() );
    pressureOpacityCheckBox->blockSignals( false );

    fillCheckBox->blockSignals( true );
    fillCheckBox->setChecked( m_lineTool->GetFill() );
    fillCheckBox->blockSignals( false );

    persistenceSpinBox->blockSignals( true );
    persistenceSpinBox->setValue( m_lineTool->GetPersistence() );
    persistenceSpinBox->blockSignals( false );
}
