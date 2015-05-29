#include "PlaybackControlerWidget.h"
#include "PlaybackControler.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QSpacerItem>
#include <QPushButton>

const int PlaybackControlerWidget::NumberOfFrameRates = 8;
const int PlaybackControlerWidget::AvailableFrameRates[8] =  \
{
	4000, // 4 spf
	2000, // 2 spf
	1000, // 1 fps
	500,  // 2 fps
	250,  // 4 fps
	167,  // 6 fps
	83,   // 12 fps
	42    // 24 fps
};

PlaybackControlerWidget::PlaybackControlerWidget( PlaybackControler * controler, QWidget * parent )
: QWidget( parent )
, m_controler( controler )
, m_frameRateIndex(6)
, m_timerId(-1)
, m_isUpdating(false)
{
	SetupUi();
	UpdateUi();
	
	connect( m_controler, SIGNAL( ModifiedSignal() ), this, SLOT(PlaybackControlerModifiedSlot()) );
	connect( m_controler, SIGNAL( StartStop(bool) ), this, SLOT(PlaybackStartStopSlot(bool)) );
    connect( m_controler, SIGNAL(FrameChanged(int)), this, SLOT(UpdateCurrentFrame()) );
}

PlaybackControlerWidget::~PlaybackControlerWidget()
{
}

void PlaybackControlerWidget::OnCurrentFrameSliderValueChanged( int value )
{
	if( !m_isUpdating )
		m_controler->SetCurrentFrame( value );
}

void PlaybackControlerWidget::OnFrameRateSliderValueChanged( int value )
{
	if( !m_isUpdating )
	{
		m_frameRateIndex = value;
		int ms = AvailableFrameRates[ m_frameRateIndex ];
		m_controler->SetFrameInterval( ms );
	}
}

void PlaybackControlerWidget::OnLoopCheckBoxToggled( bool isOn )
{
	if( !m_isUpdating )
	{
		m_controler->SetLooping( isOn );
	}
}

void PlaybackControlerWidget::PlayPauseButtonClicked()
{
	if( !m_isUpdating )
	{
		m_controler->PlayPause();
	}
}

void PlaybackControlerWidget::PlaybackControlerModifiedSlot()
{
	if( m_timerId == -1 )
		m_timerId = startTimer(0);
}

void PlaybackControlerWidget::PlaybackStartStopSlot( bool isStart )
{
	if( isStart )
	{
		if( m_timerId != -1 )
			killTimer( m_timerId );
		startTimer( 300 );
	}
	else 
	{
		if( m_timerId != -1 )
		{
			killTimer( m_timerId );
			m_timerId = -1;
		}
	}
	if( m_timerId == -1 )
		m_timerId = startTimer(0);
}

void PlaybackControlerWidget::timerEvent(QTimerEvent *event)
{
	if( !m_controler->IsPlaying() && m_timerId != -1 )
	{
		killTimer( m_timerId );
		m_timerId = -1;
	}
	UpdateUi();
}

void PlaybackControlerWidget::SetupUi()
{
	if( objectName().isEmpty() )
		setObjectName(QString::fromUtf8("PlaybackControl"));
	
	QSizePolicy mainPolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
	setSizePolicy( mainPolicy );
	
	mainLayout = new QHBoxLayout(this);
	mainLayout->setContentsMargins( 9, 9, 9, 0 );
	
	// Current frame line edit
	{
		currentFrameLineEdit = new QLineEdit(this);
		QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(currentFrameLineEdit->sizePolicy().hasHeightForWidth());
		currentFrameLineEdit->setSizePolicy(sizePolicy);
		currentFrameLineEdit->setMaximumSize(QSize(50, 16777215));
		currentFrameLineEdit->setReadOnly(true);
	}
	mainLayout->addWidget(currentFrameLineEdit);
	
	// Current frame slider
	currentFrameSlider = new QSlider(Qt::Horizontal,this);
	mainLayout->addWidget(currentFrameSlider);
	
	// Play/Pause button
	{
		playPauseButton = new QPushButton( tr("Play"), this );
		QSizePolicy policy( QSizePolicy::Fixed, QSizePolicy::Fixed );
		playPauseButton->setSizePolicy( policy );
		playPauseButton->setMinimumWidth( 81 );
	}
	mainLayout->addWidget(playPauseButton);
	
	// Loop playback checkbox
	loopCheckBox = new QCheckBox(tr("Loop"),this);
	mainLayout->addWidget(loopCheckBox);
	
	// Space
	postLoopSpacer = new QSpacerItem(28, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
	mainLayout->addItem(postLoopSpacer);
	
	// Frame rate slider
	{
		frameRateSlider = new QSlider(this);
		QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		sizePolicy.setHeightForWidth(frameRateSlider->sizePolicy().hasHeightForWidth());
		frameRateSlider->setSizePolicy(sizePolicy);
		frameRateSlider->setMaximum(7);
		frameRateSlider->setSingleStep(1);
		frameRateSlider->setOrientation(Qt::Horizontal);
		frameRateSlider->setTickPosition(QSlider::TicksBelow);
		frameRateSlider->setTickInterval(1);
	}
	mainLayout->addWidget( frameRateSlider );
	
	// Frame rate line edit
	{
		frameRateLineEdit = new QLineEdit(this);
		QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		sizePolicy.setHeightForWidth(frameRateLineEdit->sizePolicy().hasHeightForWidth());
		frameRateLineEdit->setSizePolicy(sizePolicy);
		frameRateLineEdit->setMaximumSize(QSize(50, 16777215));
		frameRateLineEdit->setReadOnly(true);
	}
	mainLayout->addWidget( frameRateLineEdit );
	
	fpsLabel = new QLabel(this);
	fpsLabel->setText(tr("fps"));
	mainLayout->addWidget( fpsLabel );
	
	connect( currentFrameSlider, SIGNAL(valueChanged(int)), this, SLOT(OnCurrentFrameSliderValueChanged(int)) );
	connect( frameRateSlider, SIGNAL(valueChanged(int)), this, SLOT(OnFrameRateSliderValueChanged(int)) );
	connect( playPauseButton, SIGNAL(clicked()), this, SLOT(PlayPauseButtonClicked()) );
	connect( loopCheckBox, SIGNAL(toggled(bool)), this, SLOT(OnLoopCheckBoxToggled(bool)) );
}

void PlaybackControlerWidget::UpdateUi()
{	
	m_isUpdating = true;
	
	// Current frame widgets
	UpdateCurrentFrame();
	
	// Play/Pause button
	if( m_controler->IsPlaying() )
		playPauseButton->setText(tr("Pause"));
	else
		playPauseButton->setText(tr("Play"));
	
	// Loop check box
	loopCheckBox->setChecked( m_controler->GetLooping() );
	
	// Frame rate slider
	frameRateSlider->setValue( m_frameRateIndex );
	
	// Frame rate line edit
	int mSecPerFrame = AvailableFrameRates[ m_frameRateIndex ];
	double fps = double(1000) / double(mSecPerFrame);
	frameRateLineEdit->setText( QString::number( fps, 'g', 2 ) );
	
	m_isUpdating = false;
}

void PlaybackControlerWidget::UpdateCurrentFrame()
{
	int numberOfFrames = m_controler->GetNumberOfFrames();
	currentFrameSlider->setRange( 0, numberOfFrames - 1 );
	
	// Current frame slider
	int currentFrame = m_controler->GetCurrentFrame();
	currentFrameSlider->setValue( currentFrame );
	
	// Current frame line edit
	currentFrameLineEdit->setText( QString::number( currentFrame ) );
}
