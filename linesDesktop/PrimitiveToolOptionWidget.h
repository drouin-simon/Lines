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
class drwLineTool;
class drwGradientWidget;

class PrimitiveToolOptionWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
    PrimitiveToolOptionWidget( drwLineTool * lineTool, QWidget * parent = 0 );
	~PrimitiveToolOptionWidget();
	
private slots:
	
	void OnColorSliderValueChanged( double );
	void pressureWidthCheckBoxStateChanged(int);
	void pressureOpacityCheckBoxStateChanged(int);
	void fillCheckBoxStateChanged(int);
	void FrameChangeRadioToggled(bool);
	void OnPersistenceSpinBoxValueChanged(int);
	void ToolModified();
	
private:
	
	void SetupUi();
	void UpdateUi();

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
