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
static const QByteArray ProgramSignature = "LINES_BROADCAST_SESSION";
static const QByteArray LinesProtocolSignature = "LINES_PROTOCOL_v1";
static const quint16 BroadcastPort = 45454;

class drwNetworkConnection : public QObject
{
    Q_OBJECT

public:

    enum NextRead{ ClientProtocolSignature, ServerProtocolSignature, PeerNameSize, PeerName, CommandHeader, CommandBody };

    drwNetworkConnection( QTcpSocket *socket, QObject * parent = 0 );
	drwNetworkConnection( QString peerUserName, QHostAddress & address, QObject *parent = 0 );
	~drwNetworkConnection();

	QString GetPeerUserName() { return m_peerUserName; }
	QHostAddress GetPeerAddress() { return m_peerAddress; }

	void Disconnect();

	static QString ComputeUserName();

signals:

	void CommandReceived( drwCommand::s_ptr command );
	void ConnectionReady( drwNetworkConnection * );
    void ConnectionLost( drwNetworkConnection *, QString errorMsg );
	
public slots:

	void SendCommand( drwCommand::s_ptr command );
	void SocketConnected();
	void SocketDisconnected();

private slots:

    void processReadyRead();
	
private:
	
    void SelfDisconnect( QString errorMsg );
    void SendProtocolSignature();
	void SendString( const QString & msg );
	
	QTcpSocket * m_socket;
	QString	m_peerUserName;
	QHostAddress m_peerAddress;
	QString m_userName;

	NextRead m_nextRead;
	int m_nextReadSize;

	drwCommand::s_ptr m_pendingCommand;
};

#endif
