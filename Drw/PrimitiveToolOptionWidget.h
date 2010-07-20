#ifndef __PrimitiveToolOptionWidget_h_
#define __PrimitiveToolOptionWidget_h_

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QSpinBox;
class QGroupBox;
class QRadioButton;
class drwEditionState;

class PrimitiveToolOptionWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
	PrimitiveToolOptionWidget( drwEditionState * editionState, QWidget * parent = 0 );
	~PrimitiveToolOptionWidget();
	
private slots:
	
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
	
	// UI elements
	QVBoxLayout * mainLayout;
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