#ifndef __DisplaySettingsWidget_h_
#define __DisplaySettingsWidget_h_

#include <QWidget>
#include "drwGlobal.h"

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QCheckBox;
class QSpinBox;
class drwDisplaySettings;
class QGroupBox;

class DisplaySettingsWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
	DisplaySettingsWidget( drwDisplaySettings * DisplaySettings, QWidget * parent = 0 );
	~DisplaySettingsWidget();
	
private slots:
	
	void OnFramesBeforeValueChanged(int);
	void OnFramesAfterValueChanged(int);
	void OnAllFrameChecked( bool );
	void OnDisplayCameraFrameChecked( bool );
	void SettingsModified();
	
private:
	
	void timerEvent(QTimerEvent *event);
	
	void SetupUi();
	void UpdateUi();
	
	int m_timerId;
	bool m_updating;
	
	drwDisplaySettings * m_displaySettings;
	
	// UI elements
	QVBoxLayout * mainLayout;
	QGroupBox	* onionSkinGroupBox;
	QVBoxLayout * onionSkinLayout;
	QHBoxLayout * framesBeforeLayout;
	QLabel		* framesBeforeLabel;
	QSpinBox	* framesBeforeSpinBox;
	QHBoxLayout * framesAfterLayout;
	QLabel		* framesAfterLabel;
	QSpinBox	* framesAfterSpinBox;
	QCheckBox	* allFramesCheckBox;
	QCheckBox	* displayCameraFrameCheckBox;
	
};

#endif