#ifndef __drwNetworkInterface_h_
#define __drwNetworkInterface_h_

#include <QObject>
#include <QHostAddress>
#include "drwCommand.h"

QT_BEGIN_NAMESPACE
class QUdpSocket;
class QTcpServer;
class QTimer;
QT_END_NAMESPACE
class drwNetworkConnection;
class drwCommand;
class Scene;
class drwCommandDatabase;

class drwNetworkInterface : public QObject
{
    Q_OBJECT

public:
	
    drwNetworkInterface( Scene * scene, drwCommandDatabase * dataBase, QObject * parent = 0 );
	void SetUserName( QString userName );
	QString GetUserName() { return m_userName; }
	void Start( bool broadcast, bool listenBroadcast );
	void Stop();
	bool isOn() { return m_isOn; }
	void GetConnectionUserNamesAndIps( QStringList & userNameList, QStringList & ipList );
	
signals:
	
	void ModifiedSignal();
	
public slots:
	
	void SendCommand( drwCommand::s_ptr command );

private slots:
	
    void ProcessReceivedBroadcastMessages();
	void Broadcast();
	void NewIncomingConnection();
	void ConnectionReady( drwNetworkConnection * );
	void ConnectionLost( drwNetworkConnection * );

private:
	
	void Reset();
	bool CanConnect( QString peerUserName, QHostAddress & peerAddress );
	bool IsSelfAddress( QHostAddress & address );
	void GatherSelfAddresses();
	
	int m_nextUserId;
	Scene * m_scene;
	drwCommandDatabase * m_commandDb;
	QString m_userName;
	bool m_isOn;
	
	QTimer     * m_broadcastTimer;
	QUdpSocket * m_broadcastSocket;
	QTcpServer * m_tcpServer;
	QList<drwNetworkConnection*> m_connections;
	
	QList<QHostAddress> m_selfBroadcastAddresses;
    QList<QHostAddress> m_selfIPAddresses;
};

#endif
