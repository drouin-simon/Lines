#ifndef __DisplaySettingsWidget_h_
#define __DisplaySettingsWidget_h_

#include <QWidget>

class LinesCore;

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QCheckBox;
class QSpinBox;
class QGroupBox;

class DisplaySettingsWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
    DisplaySettingsWidget( LinesCore * lc, QWidget * parent = 0 );
	~DisplaySettingsWidget();
	
private slots:
	
	void OnFramesBeforeValueChanged(int);
	void OnFramesAfterValueChanged(int);
    void UpdateUI();
	
private:

    void SetupUi();
    void BlockSigs( bool block );
	
    LinesCore * m_lines;
	
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
	
};

#endif
