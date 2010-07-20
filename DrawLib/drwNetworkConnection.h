#ifndef __drwNetworkConnection_h_
#define __drwNetworkConnection_h_

#include <QHostAddress>
#include <QString>
#include "drwCommand.h"

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE
class drwToolbox;
class Scene;

static const int MaxBufferSize = 1024000;
static const int ConnectionPort = 45455;

class drwNetworkConnection : public QObject
{
    Q_OBJECT

public:
    drwNetworkConnection( int userId, QString userName, Scene * scene, QObject *parent = 0, QTcpSocket *socket = 0 );
	drwNetworkConnection( int userId, QString userName, QString peerUserName, QHostAddress & address, Scene * scene, QObject *parent = 0 );
	~drwNetworkConnection();
	QString GetPeerUserName() { return m_peerUserName; }
	QHostAddress GetPeerAddress() { return m_peerAddress; }
	void Disconnect();

signals:
    void CommandExecutedSignal( drwCommand::s_ptr command );
	void ConnectionReady( drwNetworkConnection * );
	void ConnectionLost( drwNetworkConnection * );
	
public slots:
	void SendCommand( drwCommand::s_ptr command );
	void SocketConnected();
	void SocketDisconnected();

private slots:
    void processReadyRead();
	void CommandExecuted( drwCommand::s_ptr command );
	
private:
	
	void CreateToolbox();
	void SendString( const QString & msg );
	QString ReadString();
	
	int m_userId;
	QTcpSocket * m_socket;
	QString	m_peerUserName;
	QHostAddress m_peerAddress;
	QString m_userName;
	bool m_ready;
	Scene * m_scene;
	drwToolbox * m_observer;
	drwCommand::s_ptr m_pendingCommand;
};

#endif
