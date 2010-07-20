#include <QtNetwork>
#include "drwNetworkInterface.h"
#include "drwNetworkConnection.h"
#include "drwCommandDatabase.h"

const QByteArray ProgramSignature = "DRAW_FRIEND_SEARCH";
const quint16 BroadcastPort = 45454;
const int BroadcastTimeout = 2000;

drwNetworkInterface::drwNetworkInterface( Scene * scene, drwCommandDatabase * dataBase, QObject * parent ) 
: QObject(parent)
, m_nextUserId( 1 )
, m_scene( scene )
, m_commandDb( dataBase )
, m_isOn(false)
, m_broadcastTimer(0)
, m_broadcastSocket(0)
, m_tcpServer(0)
{
	m_broadcastSocket = new QUdpSocket(this);
	connect(m_broadcastSocket, SIGNAL(readyRead()), this, SLOT(ProcessReceivedBroadcastMessages()));
	
	m_broadcastTimer = new QTimer(this);
	connect( m_broadcastTimer, SIGNAL(timeout()), this, SLOT(Broadcast()) );
	
	m_tcpServer = new QTcpServer(this);
	connect( m_tcpServer, SIGNAL(newConnection()), this, SLOT(NewIncomingConnection()) );
}

void drwNetworkInterface::SetUserName( QString userName )
{
	if( !m_isOn )
	{
		m_userName = userName;
		emit ModifiedSignal();
	}
}

void drwNetworkInterface::Start( bool broadcast, bool listenBroadcast )
{
	// Build the list of addresse we have on this machine
	GatherSelfAddresses();
	
	// Build the socket that is used to listen to broadcast messages emitted by peers (to collect new peers).
	if( listenBroadcast )
		m_broadcastSocket->bind(BroadcastPort);
	
	if( broadcast )
	{
		// Broadcast a first datagram and setup a timer to broadcast at regular intervals
		m_broadcastTimer->start( BroadcastTimeout );
		Broadcast();
	
		// Setup a TCP server to listen for new connections initiated by peers who received our broadcast message
		m_tcpServer->listen( QHostAddress::Any, ConnectionPort );
	}
	m_isOn = true;
	emit ModifiedSignal();
}

void drwNetworkInterface::Stop()
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

void drwNetworkInterface::ProcessReceivedBroadcastMessages()
{
    while (m_broadcastSocket->hasPendingDatagrams()) 
	{
		// Read a datagram
        QByteArray datagram;
        datagram.resize(m_broadcastSocket->pendingDatagramSize());
		QHostAddress address;
		quint16 port;
        m_broadcastSocket->readDatagram(datagram.data(), datagram.size(), &address, &port);
		
		// Verify datagram signature and extract peer username
		QList<QByteArray> dataTokens = datagram.split('=');
		if( dataTokens.at(0) == ProgramSignature )
		{
			QString peerUserName = dataTokens.at(1);
			
			if( CanConnect( peerUserName, address ) )
			{
				// Create a new connection
				drwNetworkConnection * connection = new drwNetworkConnection( m_nextUserId++, m_userName, peerUserName, address, m_scene, this );
				connect( connection, SIGNAL(ConnectionReady(drwNetworkConnection*)), this, SLOT(ConnectionReady(drwNetworkConnection*)) );
				connect( connection, SIGNAL(ConnectionLost(drwNetworkConnection*)), this, SLOT(ConnectionLost(drwNetworkConnection*)) );
				m_connections.push_back( connection );
			}
		}
    }
}

void drwNetworkInterface::Broadcast()
{
	QByteArray datagram = ProgramSignature + "=" + m_userName.toUtf8();
    m_broadcastSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, BroadcastPort);
}

void drwNetworkInterface::NewIncomingConnection()
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

void drwNetworkInterface::ConnectionReady( drwNetworkConnection * connection )
{
	connect( connection, SIGNAL(CommandExecutedSignal( drwCommand::s_ptr )), m_commandDb, SLOT( PushCommand( drwCommand::s_ptr ) ), Qt::DirectConnection );
	emit ModifiedSignal();
}

void drwNetworkInterface::ConnectionLost( drwNetworkConnection * connection )
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

void drwNetworkInterface::SendCommand( drwCommand::s_ptr command )
{
	for( int i = 0; i < m_connections.size(); ++i )
	{
		m_connections.at( i )->SendCommand( command );
	}
}

bool drwNetworkInterface::CanConnect( QString peerUserName, QHostAddress & peerAddress )
{
	// first check we are receiving one of our own broadcast message
	if( peerUserName == m_userName && IsSelfAddress( peerAddress ) )
		return false;
	
	// now check that there isn't already a connection to that peer
	for( int i = 0; i < m_connections.size(); ++i )
	{
		if( m_connections.at(i)->GetPeerUserName() == peerUserName && m_connections.at(i)->GetPeerAddress() == peerAddress )
			return false;
	}
	
	return true;
}

bool drwNetworkInterface::IsSelfAddress( QHostAddress & address )
{
	foreach( QHostAddress selfAddress, m_selfIPAddresses )
	{
		if( selfAddress == address )
			return true;
	}
	return false;
}

void drwNetworkInterface::GatherSelfAddresses()
{
	m_selfBroadcastAddresses.clear();
    m_selfIPAddresses.clear();
    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) 
	{
        foreach (QNetworkAddressEntry entry, interface.addressEntries()) 
		{
            QHostAddress broadcastAddress = entry.broadcast();
            if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost) 
			{
                m_selfBroadcastAddresses << broadcastAddress;
                m_selfIPAddresses << entry.ip();
            }
        }
    }
}
