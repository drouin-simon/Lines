#ifndef __drwNetworkServer_h_
#define __drwNetworkServer_h_

#include <QObject>
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
	bool isOn() { return m_isOn; }
	void GetConnectionUserNamesAndIps( QStringList & userNameList, QStringList & ipList );
	
signals:
	
	void ModifiedSignal();
	
public slots:
	
	void SendCommand( drwCommand::s_ptr command );
	
private slots:
	
	void Broadcast();
	void NewIncomingConnection();
	void ConnectionReady( drwNetworkConnection * );
	void ConnectionLost( drwNetworkConnection * );
	void CommandReceived( drwCommand::s_ptr com );
	
private:
	
	void Reset();
	
	int m_nextUserId;
	drwCommandDispatcher * m_dispatcher;
	QString m_userName;
	bool m_isOn;
	
	QTimer     * m_broadcastTimer;
	QUdpSocket * m_broadcastSocket;
	QTcpServer * m_tcpServer;
	QList<drwNetworkConnection*> m_connections;
	
};

#endif