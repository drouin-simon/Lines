#ifndef __PrimitiveToolOptionWidget_h_
#define __PrimitiveToolOptionWidget_h_

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QSpinBox;
class QGroupBox;
class QRadioButton;
class QCheckBox;
class drwEditionState;
class drwLineTool;
class drwGradientWidget;

class PrimitiveToolOptionWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
	PrimitiveToolOptionWidget( drwEditionState * editionState, drwLineTool * lineTool, QWidget * parent = 0 );
	~PrimitiveToolOptionWidget();
	
private slots:
	
	void OnColorSliderValueChanged( double );
	void pressureWidthCheckBoxStateChanged(int);
	void pressureOpacityCheckBoxStateChanged(int);
	void FrameChangeRadioToggled(bool);
	void OnPersistenceSpinBoxValueChanged(int);
	void ToolModified();
	
private:
	
	void timerEvent(QTimerEvent *event);
	
	void SetupUi();
	void UpdateUi();
	
	int m_timerId;
	bool m_updating;
	
	drwEditionState * m_editionState;
	drwLineTool * m_lineTool;
	
	// UI elements
	QVBoxLayout * mainLayout;
	drwGradientWidget * gradientWidget;
	QCheckBox   * pressureWidthCheckBox;
	QCheckBox   * pressureOpacityCheckBox;
	QCheckBox   * fillCheckBox;
	QGroupBox	* frameChangeModeGroupBox;
	QVBoxLayout * frameChangeModeLayout;
	QRadioButton* manualFrameChangeRadio;
	QRadioButton* jumpAfterFrameChangeRadio;
	QRadioButton* playFrameChangeRadio;
	QHBoxLayout * persistenceLayout;
	QLabel		* persistenceLabel;
	QSpinBox	* persistenceSpinBox;
	
};

#endif
