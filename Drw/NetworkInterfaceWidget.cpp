#include "NetworkInterfaceWidget.h"
#include "drwNetworkThread.h"
#include "drwNetworkConnection.h"
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLabel>


NetworkInterfaceWidget::NetworkInterfaceWidget( drwNetworkThread * netThread, QWidget * parent ) 
: QWidget( parent )
, m_timerId(-1)
, m_needUpdate(true)
, m_updating(false)
, m_networkThread( netThread )
{
	// Setup the gui elements
	SetupUi();
	
	connect( m_networkThread, SIGNAL(ModifiedSignal()), this, SLOT(InterfaceModified()) );
	
	// make sure timerEvent is called on idle
	UpdateUi();
}

NetworkInterfaceWidget::~NetworkInterfaceWidget()
{
	disconnect(m_networkThread, SIGNAL(ModifiedSignal()), this, SLOT(InterfaceModified()) );
}

void NetworkInterfaceWidget::OnStartButtonClicked()
{
	if( m_networkThread->isRunning() )
		m_networkThread->StopNetwork();
	else
		m_networkThread->StartNetwork();
}

void NetworkInterfaceWidget::OnLocalUserEditEditingFinished()
{
	if( !m_updating )
	{
		QString userName = localUserEdit->text();
		m_networkThread->SetUserName( userName );
	}
}

void NetworkInterfaceWidget::InterfaceModified()
{
	if( m_timerId == -1 )
		m_timerId = startTimer(0);
	m_needUpdate = true;
}

void NetworkInterfaceWidget::UpdateUi()
{
	m_updating = true;
	if( m_networkThread->isRunning() )
	{
		startButton->setText("Stop");
		localUserEdit->setEnabled( false );
	}
	else
	{
		startButton->setText("Start");
		localUserEdit->setEnabled( true );
	}
	localUserEdit->setText( m_networkThread->GetUserName() );
	
	connectionsTable->clear();
	connectionsTable->setRowCount( 0 );
	connectionsTable->setHorizontalHeaderItem( 0, new QTableWidgetItem( tr("Username") ) );
	connectionsTable->setHorizontalHeaderItem( 1, new QTableWidgetItem( tr("IP") ) );
	connectionsTable->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
	connectionsTable->setShowGrid(false);
	QStringList users;
	QStringList ips;
	m_networkThread->GetConnectionUserNamesAndIps( users, ips );
	connectionsTable->setRowCount( users.size() );
	for( int i = 0; i < users.size(); ++i )
	{
		QTableWidgetItem * peerUserNameItem = new QTableWidgetItem( users.at(i) );
		connectionsTable->setItem( 0, 0, peerUserNameItem );
		QTableWidgetItem * peerAddressItem = new QTableWidgetItem( ips.at(i) );
		connectionsTable->setItem( 0, 1, peerAddressItem );
	}
	m_updating = false;
	m_needUpdate = false;
}

void NetworkInterfaceWidget::timerEvent(QTimerEvent *event)
{
	killTimer( m_timerId );
	m_timerId = -1;
	if( m_needUpdate )
		UpdateUi();
}

void NetworkInterfaceWidget::SetupUi()
{
	if( this->objectName().isEmpty() )
		this->setObjectName(QString::fromUtf8("NetworkInterfaceWidget"));
	
	QSizePolicy mainPolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
	setSizePolicy( mainPolicy );
	
	verticalLayout = new QVBoxLayout( this );
	
	{
		localUserLayout = new QHBoxLayout();
		localUserLabel = new QLabel;
		localUserLabel->setText(tr("My User Name: "));
		localUserLayout->addWidget( localUserLabel );
		localUserEdit = new QLineEdit;
		localUserLayout->addWidget( localUserEdit );
	}
	verticalLayout->addLayout( localUserLayout );
	
	{
		connectionsLabelLayout = new QHBoxLayout;
		connectionsLabel = new QLabel;
		connectionsLabel->setText(tr("Connections: "));
		connectionsLabelLayout->addWidget( connectionsLabel );
		connectionsLabelSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		connectionsLabelLayout->addItem( connectionsLabelSpacer );
	}
	verticalLayout->addLayout( connectionsLabelLayout );
	
	{
		connectionsTable = new QTableWidget;
		connectionsTable->setColumnCount(2);
		QTableWidgetItem * tableWidgetItem1 = new QTableWidgetItem();
		connectionsTable->setHorizontalHeaderItem(0, tableWidgetItem1 );
		QTableWidgetItem * tableWidgetItem2 = new QTableWidgetItem();
		connectionsTable->setHorizontalHeaderItem(1, tableWidgetItem2 );
	}
	verticalLayout->addWidget(connectionsTable);
	
	{
		startButtonLayout = new QHBoxLayout();
		startButtonLeftSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		startButtonLayout->addItem(startButtonLeftSpacer);
		startButton = new QPushButton(tr("Start"),this);
		QSizePolicy policy( QSizePolicy::Fixed, QSizePolicy::Fixed );
		startButton->setSizePolicy( policy );
		startButton->resize( 81, 32 );
		startButton->setMaximumWidth( 150 );
		startButton->setMinimumHeight( 32 );
		startButtonLayout->addWidget(startButton);
		startButtonRightSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		startButtonLayout->addItem(startButtonRightSpacer);
	}
	verticalLayout->addLayout( startButtonLayout );
	
	// setup connections
	connect( startButton, SIGNAL( clicked() ), this, SLOT( OnStartButtonClicked() ) );
	connect( localUserEdit, SIGNAL( editingFinished() ), this, SLOT( OnLocalUserEditEditingFinished() ) );
}