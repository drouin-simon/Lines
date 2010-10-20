#include "drwNetworkServer.h"
#include <QtNetwork>
#include "drwNetworkConnection.h"
#include "drwCommandDispatcher.h"

const int BroadcastTimeout = 2000;

drwNetworkServer::drwNetworkServer( drwCommandDispatcher * dispatcher, QObject * parent ) 
: QObject(parent)
, m_nextUserId( 1 )
, m_dispatcher( dispatcher )
, m_isOn(false)
, m_broadcastTimer(0)
, m_broadcastSocket(0)
, m_tcpServer(0)
{
	m_broadcastSocket = new QUdpSocket(this);
	
	m_broadcastTimer = new QTimer(this);
	connect( m_broadcastTimer, SIGNAL(timeout()), this, SLOT(Broadcast()) );
	
	m_tcpServer = new QTcpServer(this);
	connect( m_tcpServer, SIGNAL(newConnection()), this, SLOT(NewIncomingConnection()) );
}

void drwNetworkServer::Start()
{
	// Build the list of addresse we have on this machine
	GatherSelfAddresses();
	
	// Broadcast a first datagram and setup a timer to broadcast at regular intervals
	m_broadcastTimer->start( BroadcastTimeout );
	Broadcast();
		
	// Setup a TCP server to listen for new connections initiated by peers who received our broadcast message
	m_tcpServer->listen( QHostAddress::Any, ConnectionPort );
	
	m_isOn = true;
	emit ModifiedSignal();
}

void drwNetworkServer::Stop()
{
	if( m_isOn )
	{
		m_broadcastTimer->stop();
		m_tcpServer->close();
		m_broadcastSocket->close();
		while ( m_connections.size() > 0 ) 
		{
			m_connections.front()->Disconnect();
		}
		m_isOn = false;
		emit ModifiedSignal();
	}
}

void drwNetworkInterface::GetConnectionUserNamesAndIps( QStringList & userNameList, QStringList & ipList )
{
	for (int i = 0; i < m_connections.size(); ++i) 
	{
		userNameList.push_back( m_connections.at(i)->GetPeerUserName() );
		ipList.push_back( m_connections.at(i)->GetPeerAddress().toString() );
	}
}

void drwNetworkInterface::Broadcast()
{
	QByteArray datagram = ProgramSignature + "=" + m_userName.toUtf8();
    m_broadcastSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, BroadcastPort);
}

void drwNetworkServer::NewIncomingConnection()
{
	while( m_tcpServer->hasPendingConnections() )
	{
		QTcpSocket * sock = m_tcpServer->nextPendingConnection();
		drwNetworkConnection * connection = new drwNetworkConnection( m_nextUserId++, m_userName, m_scene, this, sock );
		connect( connection, SIGNAL(ConnectionReady(drwNetworkConnection*)), this, SLOT(ConnectionReady(drwNetworkConnection*)) );
		connect( connection, SIGNAL(ConnectionLost(drwNetworkConnection*)), this, SLOT(ConnectionLost(drwNetworkConnection*)) );
		m_connections.push_back( connection );
	}
}

void drwNetworkServer::ConnectionReady( drwNetworkConnection * connection )
{
	connect( connection, SIGNAL(CommandReceived( drwCommand::s_ptr )), this, SLOT( CommandReceived( drwCommand::s_ptr ) ), Qt::DirectConnection );
	emit ModifiedSignal();
}

void drwNetworkServer::CommandReceived( drwCommand::s_ptr com )
{
	// send it to all other connections
	// todo: check user id and send it to all connections that have a different id
	
	// execute it here (including storing in database)
	m_dispatcher->ExecuteCommand( com );
}

void drwNetworkServer::ConnectionLost( drwNetworkConnection * connection )
{
	int index = -1;
	for( int i = 0; i < m_connections.size(); ++i )
	{
		if( m_connections.at(i) == connection )
		{
			index = i;
			break;
		}
	}
	
	if( index != -1 )
	{
		m_connections.removeAt( index );
		connection->deleteLater();
		emit ModifiedSignal();
	}
}

// Local command, must be sent to all connected clients
void drwNetworkServer::SendCommand( drwCommand::s_ptr command )
{
	for( int i = 0; i < m_connections.size(); ++i )
	{
		m_connections.at( i )->SendCommand( command );
	}
}

