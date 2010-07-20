#ifndef __PlaybackControlerWidget_h_
#define __PlaybackControlerWidget_h_

#include <QWidget>

class PlaybackControler;
class QHBoxLayout;
class QLineEdit;
class QSlider;
class QCheckBox;
class QSpacerItem;
class QLabel;
class QPushButton;

class PlaybackControlerWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
	PlaybackControlerWidget( PlaybackControler * controler, QWidget * parent = 0 );
	~PlaybackControlerWidget();
	
private slots:
	
	// Ui-connected slots
	void OnCurrentFrameSliderValueChanged( int value );
	void OnFrameRateSliderValueChanged( int value );
	void OnLoopCheckBoxToggled( bool isOn );
	void PlayPauseButtonClicked();
	
	// Controler-connected slots
	void PlaybackControlerModifiedSlot();
	void PlaybackStartStopSlot( bool isStart );

private:
	
	void timerEvent(QTimerEvent *event);

	void SetupUi();
	void UpdateUi();
	void UpdateCurrentFrame();
	
	PlaybackControler * m_controler;
	int m_frameRateIndex;
	int m_timerId;
	bool m_isUpdating;
	static const int NumberOfFrameRates;
	static const int AvailableFrameRates[8];	
	
	QHBoxLayout * mainLayout;
    QLineEdit	* currentFrameLineEdit;
    QSlider		* currentFrameSlider;
	QPushButton * playPauseButton;
    QCheckBox	* loopCheckBox;
    QSpacerItem * postLoopSpacer;
    QSlider		* frameRateSlider;
    QLineEdit	* frameRateLineEdit;
    QLabel		* fpsLabel;
};

#endif