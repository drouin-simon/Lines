#ifndef __drwNetworkServer_h_
#define __drwNetworkServer_h_

#include <QObject>
#include <QMap>
#include "drwCommand.h"

QT_BEGIN_NAMESPACE
class QUdpSocket;
class QTcpServer;
class QTimer;
QT_END_NAMESPACE
class drwNetworkConnection;
class drwCommandDispatcher;

class drwNetworkServer : public QObject
{
	
	Q_OBJECT
	
public:
	
	drwNetworkServer( drwCommandDispatcher * dispatcher, QObject * parent = 0 );
	~drwNetworkServer();
	
	void Start();
	void Stop();
	void GetConnectionUserNamesAndIps( QStringList & userNameList, QStringList & ipList );
	
public slots:
	
	void SendCommand( drwCommand::s_ptr command );
	
private slots:
	
	void Broadcast();
	void NewIncomingConnection();
	void ConnectionReadySlot( drwNetworkConnection * );
	void ConnectionLost( drwNetworkConnection * );
    void CommandReceivedSlot( drwCommand::s_ptr com );
	
private:
	
	void Reset();
	
	QString m_userName;
	bool m_isOn;
	
	QTimer     * m_broadcastTimer;
	QUdpSocket * m_broadcastSocket;
	QTcpServer * m_tcpServer;
	typedef QMap< drwNetworkConnection*, int > ConnectionsContainer;
	ConnectionsContainer m_connections;

	drwCommandDispatcher * m_dispatcher;
	
};

#endif
