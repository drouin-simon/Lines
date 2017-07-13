#include "PlaybackControlerWidget.h"
#include "LinesCore.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QSpacerItem>
#include <QToolButton>

const int PlaybackControlerWidget::NumberOfFrameRates = 9;
const int PlaybackControlerWidget::AvailableFrameRates[9] =  \
{
	4000, // 4 spf
	2000, // 2 spf
	1000, // 1 fps
	500,  // 2 fps
	250,  // 4 fps
	167,  // 6 fps
    125,  // 8 fps
	83,   // 12 fps
	42    // 24 fps
};

int PlaybackControlerWidget::IndexFromFrameRate( int ms )
{
    for( int i = 0; i < PlaybackControlerWidget::NumberOfFrameRates; ++i )
    {
        if( PlaybackControlerWidget::AvailableFrameRates[i] == ms )
            return i;
    }
    return -1;
}

PlaybackControlerWidget::PlaybackControlerWidget( LinesCore * lc, QWidget * parent )
: QWidget( parent )
, m_lines( lc )
, playIcon("://icons-60x60/play.png")
, pauseIcon("://icons-60x60/pause.png")
{
	SetupUi();
	UpdateUi();
	
    connect( m_lines, SIGNAL( PlaybackStartStop(bool) ), this, SLOT(PlaybackStartStopSlot(bool)) );
    connect( m_lines, SIGNAL( PlaybackSettingsChangedSignal() ), this, SLOT(UpdateUi()) );
}

PlaybackControlerWidget::~PlaybackControlerWidget()
{
}

void PlaybackControlerWidget::SetHideFrameRate( bool hide )
{
    frameRateLineEdit->setHidden( hide );
    frameRateSlider->setHidden( hide );
    fpsLabel->setHidden( hide );
}

void PlaybackControlerWidget::OnCurrentFrameSliderValueChanged( int value )
{
    m_lines->SetCurrentFrame( value );
}

void PlaybackControlerWidget::OnFrameRateSliderValueChanged( int value )
{
    int ms = AvailableFrameRates[ value ];
    m_lines->SetFrameInterval( ms );
}

void PlaybackControlerWidget::PlayPauseButtonClicked()
{
    m_lines->PlayPause();
}

void PlaybackControlerWidget::PlaybackStartStopSlot( bool isStart )
{
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
        playPauseButton = new QToolButton( this );
        playPauseButton->setIcon( playIcon );
	}
    mainLayout->addWidget( playPauseButton );
	
	// Space
	postLoopSpacer = new QSpacerItem(28, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
	mainLayout->addItem(postLoopSpacer);
	
	// Frame rate slider
	{
		frameRateSlider = new QSlider(this);
		QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		sizePolicy.setHeightForWidth(frameRateSlider->sizePolicy().hasHeightForWidth());
		frameRateSlider->setSizePolicy(sizePolicy);
        frameRateSlider->setMaximum( NumberOfFrameRates - 1 );
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
}

void PlaybackControlerWidget::UpdateUi()
{		
	// Current frame widgets
	UpdateCurrentFrame();
	
	// Play/Pause button
    if( m_lines->IsPlaying() )
        playPauseButton->setIcon( pauseIcon );
	else
        playPauseButton->setIcon( playIcon );

    //
    int mSecPerFrame = m_lines->GetFrameInterval();
    int frameRateIndex = IndexFromFrameRate( mSecPerFrame );
    Q_ASSERT( frameRateIndex != -1 );
	
	// Frame rate slider
    frameRateSlider->blockSignals( true );
    frameRateSlider->setValue( frameRateIndex );
    frameRateSlider->blockSignals( false );
	
	// Frame rate line edit
	double fps = double(1000) / double(mSecPerFrame);
	frameRateLineEdit->setText( QString::number( fps, 'g', 2 ) );
}

void PlaybackControlerWidget::UpdateCurrentFrame()
{
    currentFrameSlider->blockSignals( true );

    int numberOfFrames = m_lines->GetNumberOfFrames();
	currentFrameSlider->setRange( 0, numberOfFrames - 1 );
	
	// Current frame slider
    int currentFrame = m_lines->GetCurrentFrame();
	currentFrameSlider->setValue( currentFrame );

    currentFrameSlider->blockSignals( false );
	
	// Current frame line edit
	currentFrameLineEdit->setText( QString::number( currentFrame ) );
}
