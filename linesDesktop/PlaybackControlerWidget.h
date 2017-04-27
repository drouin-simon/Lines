#ifndef __PlaybackControlerWidget_h_
#define __PlaybackControlerWidget_h_

#include <QWidget>
#include <QIcon>

class LinesCore;
class QHBoxLayout;
class QLineEdit;
class QSlider;
class QCheckBox;
class QSpacerItem;
class QLabel;
class QToolButton;

class PlaybackControlerWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
    PlaybackControlerWidget( LinesCore * lc, QWidget * parent = 0 );
	~PlaybackControlerWidget();

    void SetHideFrameRate( bool hide );
	
private slots:
	
	// Ui-connected slots
	void OnCurrentFrameSliderValueChanged( int value );
	void OnFrameRateSliderValueChanged( int value );
	void PlayPauseButtonClicked();
	
	// Controler-connected slots
	void PlaybackStartStopSlot( bool isStart );
    void UpdateUi();
    void UpdateCurrentFrame();

private:

	void SetupUi();
	
    LinesCore * m_lines;
	int m_frameRateIndex;
	static const int NumberOfFrameRates;
	static const int AvailableFrameRates[8];	
	
	QHBoxLayout * mainLayout;
    QLineEdit	* currentFrameLineEdit;
    QSlider		* currentFrameSlider;
    QToolButton * playPauseButton;
    QIcon         playIcon;
    QIcon         pauseIcon;
    QSpacerItem * postLoopSpacer;
    QSlider		* frameRateSlider;
    QLineEdit	* frameRateLineEdit;
    QLabel		* fpsLabel;
};

#endif
