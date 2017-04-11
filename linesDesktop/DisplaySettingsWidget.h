#ifndef __DisplaySettingsWidget_h_
#define __DisplaySettingsWidget_h_

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QCheckBox;
class QSpinBox;
class drwDrawingWidget;
class QGroupBox;

class DisplaySettingsWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
    DisplaySettingsWidget( drwDrawingWidget * widget, QWidget * parent = 0 );
	~DisplaySettingsWidget();
	
private slots:
	
	void OnFramesBeforeValueChanged(int);
	void OnFramesAfterValueChanged(int);
    void UpdateUI();
	
private:

    void SetupUi();
    void BlockSigs( bool block );
	
    drwDrawingWidget * m_drawingWidget;
	
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