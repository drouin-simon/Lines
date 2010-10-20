#ifndef __drwNetworkConnection_h_
#define __drwNetworkConnection_h_

#include <QHostAddress>
#include <QString>
#include "drwCommand.h"

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

static const int MaxBufferSize = 1024000;
static const int ConnectionPort = 45455;
static const QByteArray ProgramSignature = "DRAW_FRIEND_SEARCH";
static const quint16 BroadcastPort = 45454;

class drwNetworkConnection : public QObject
{
    Q_OBJECT

public:
    drwNetworkConnection( QString userName, QTcpSocket *socket, QObject * parent = 0 );
	drwNetworkConnection( QString userName, QString peerUserName, QHostAddress & address, QObject *parent = 0 );
	~drwNetworkConnection();
	QString GetPeerUserName() { return m_peerUserName; }
	QHostAddress GetPeerAddress() { return m_peerAddress; }
	void Disconnect();

signals:
	void CommandReceived( drwCommand::s_ptr command );
	void ConnectionReady( drwNetworkConnection * );
	void ConnectionLost( drwNetworkConnection * );
	
public slots:
	void SendCommand( drwCommand::s_ptr command );
	void SocketConnected();
	void SocketDisconnected();

private slots:
    void processReadyRead();
	
private:
	
	void SendString( const QString & msg );
	QString ReadString();
	
	QTcpSocket * m_socket;
	QString	m_peerUserName;
	QHostAddress m_peerAddress;
	QString m_userName;
	bool m_ready;
	drwCommand::s_ptr m_pendingCommand;
};

#endif
