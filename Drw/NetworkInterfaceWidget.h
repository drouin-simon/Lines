#ifndef __NetworkInterfaceWidget_h_
#define __NetworkInterfaceWidget_h_

#include <QWidget>
#include "macros.h"
#include "drwGlobal.h"

class QPushButton;
class QTableWidget;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;
class QLabel;
class QLineEdit;
class drwNetworkThread;

class NetworkInterfaceWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
	NetworkInterfaceWidget( drwNetworkThread * netInterface, QWidget * parent = 0 );
	~NetworkInterfaceWidget();

private slots:
	
	void OnStartButtonClicked();
	void OnLocalUserEditEditingFinished();
	void InterfaceModified();
	
private:
	
	void timerEvent(QTimerEvent *event);
	
	void SetupUi();
	void UpdateUi();
	
	int m_timerId;
	bool m_needUpdate;
	bool m_updating;
	
	drwNetworkThread * m_networkThread;

	// UI elements
	QVBoxLayout * verticalLayout;
	QHBoxLayout * startButtonLayout;
	QSpacerItem * startButtonLeftSpacer;
	QPushButton * startButton;
	QSpacerItem * startButtonRightSpacer;
	QHBoxLayout * localUserLayout;
	QLabel		* localUserLabel;
	QLineEdit   * localUserEdit;
	QHBoxLayout * connectionsLabelLayout;
	QLabel		* connectionsLabel;
	QSpacerItem * connectionsLabelSpacer;
	QTableWidget* connectionsTable;
	
};

#endif