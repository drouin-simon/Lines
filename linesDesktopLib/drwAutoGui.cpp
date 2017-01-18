#include "drwAutoGui.h"
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QLayout>

drwAutoGui::drwAutoGui( QObject * target, QWidget * parent ) : QWidget( parent ), m_target( target )
{
    m_guiLayout = new QVBoxLayout;
    m_guiLayout->setSpacing( 5 );
    m_guiLayout->addStretch();
    this->setLayout( m_guiLayout );
}

void drwAutoGui::AddIntSliderParam( QString paramName, int minVal, int maxVal )
{
    IntSliderGuiParam * param = new IntSliderGuiParam( this, paramName, minVal, maxVal );
    m_params.push_back( param );
}

void drwAutoGui::AddDoubleSliderParam( QString paramName, double minVal, double maxVal )
{
    DoubleSliderGuiParam * param = new DoubleSliderGuiParam( this, paramName, minVal, maxVal );
    m_params.push_back( param );
}

void drwAutoGui::TargetModified()
{
    for( int i = 0; i < m_params.size(); ++i )
        m_params[i]->Update();
}

void drwAutoGui::AddChildLayout( QHBoxLayout * childLayout )
{
    m_guiLayout->insertItem( m_guiLayout->count() - 1, childLayout );
}

// ----------------------------------------------------------------
// IntSlider
// ----------------------------------------------------------------
IntSliderGuiParam::IntSliderGuiParam( drwAutoGui * parent, QString name, int minVal, int maxVal )
    : GuiParam(parent,name)
{
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setSpacing( 10 );
    parent->AddChildLayout( layout );

    QLabel * label = new QLabel( parent );
    label->setText( name );
    label->setMinimumWidth( 70 );
    label->setMaximumWidth( 70 );
    layout->addWidget( label );

    m_slider = new QSlider( Qt::Horizontal, parent );
    m_slider->setRange( minVal, maxVal );
    m_slider->setMinimumWidth( 150 );
    layout->addWidget( m_slider );
    connect( m_slider, SIGNAL(valueChanged(int)), this, SLOT(OnSliderValueChanged(int)) );

    m_lineEdit = new QLineEdit( parent );
    m_lineEdit->setReadOnly( true );
    m_lineEdit->setMinimumWidth( 40 );
    m_lineEdit->setMaximumWidth( 40 );
    layout->addWidget( m_lineEdit );
    
    Update();
}

void IntSliderGuiParam::OnSliderValueChanged( int value )
{
    m_autoGui->GetTarget()->setProperty( GetValueName(), QVariant(value) );
}

void IntSliderGuiParam::Update()
{
    m_slider->blockSignals( true );
    int value = m_autoGui->GetTarget()->property( GetValueName() ).toInt();
    m_slider->setValue( value );
    m_slider->blockSignals( false );
    m_lineEdit->setText( QString::number(value) );
}

const char * IntSliderGuiParam::GetValueName()
{
    return m_name.toUtf8().data();
}

// ----------------------------------------------------------------
// DoubleSlider
// ----------------------------------------------------------------
DoubleSliderGuiParam::DoubleSliderGuiParam( drwAutoGui * parent, QString name, double minVal, double maxVal )
: GuiParam(parent,name)
{
    m_range[0] = minVal;
    m_range[1] = maxVal;
    
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setSpacing( 10 );
    parent->AddChildLayout( layout );
    
    QLabel * label = new QLabel( parent );
    label->setText( name );
    label->setMinimumWidth( 70 );
    label->setMaximumWidth( 70 );
    layout->addWidget( label );
    
    m_slider = new QSlider( Qt::Horizontal, parent );
    m_slider->setRange( 0, 100 );
    m_slider->setMinimumWidth( 150 );
    layout->addWidget( m_slider );
    connect( m_slider, SIGNAL(valueChanged(int)), this, SLOT(OnSliderValueChanged(int)) );
    
    m_lineEdit = new QLineEdit( parent );
    m_lineEdit->setReadOnly( true );
    m_lineEdit->setMinimumWidth( 40 );
    m_lineEdit->setMaximumWidth( 40 );
    layout->addWidget( m_lineEdit );
    
    Update();
}

void DoubleSliderGuiParam::OnSliderValueChanged( int value )
{
    double paramValue = SliderValueToParamValue( value );
    m_autoGui->GetTarget()->setProperty( GetValueName(), QVariant(paramValue) );
}

void DoubleSliderGuiParam::Update()
{
    m_slider->blockSignals( true );
    double value = m_autoGui->GetTarget()->property( GetValueName() ).toDouble();
    m_slider->setValue( ParamValueToSliderValue( value ) );
    m_slider->blockSignals( false );
    m_lineEdit->setText( QString::number(value,'f',2) );
}

const char * DoubleSliderGuiParam::GetValueName()
{
    return m_name.toUtf8().data();
}

double DoubleSliderGuiParam::SliderValueToParamValue( int sliderValue )
{
    double percent = (double)sliderValue / 100.0;
    double paramVal = m_range[0] + (m_range[1] - m_range[0]) * percent;
    return paramVal;
}

int DoubleSliderGuiParam::ParamValueToSliderValue( double paramValue )
{
    int sliderVal = (int)( ( paramValue - m_range[0] ) / ( m_range[1] - m_range[0] ) * 100 );
    return sliderVal;
}

