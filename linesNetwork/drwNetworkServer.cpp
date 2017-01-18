#include "drwNetworkServer.h"
#include <QtNetwork>
#include "drwNetworkConnection.h"
#include "drwCommandDatabase.h"
#include "drwCommandDispatcher.h"

const int BroadcastTimeout = 2000;

drwNetworkServer::drwNetworkServer( drwCommandDispatcher * dispatcher, QObject * parent )
: QObject(parent)
, m_isOn(false)
, m_broadcastTimer(0)
, m_broadcastSocket(0)
, m_tcpServer(0)
, m_dispatcher( dispatcher )
{
	m_userName = drwNetworkConnection::ComputeUserName();
	m_translatedLocalUserId = m_dispatcher->RequestNewUserId();
	
	m_broadcastSocket = new QUdpSocket(this);
	
	m_broadcastTimer = new QTimer(this);
	connect( m_broadcastTimer, SIGNAL(timeout()), this, SLOT(Broadcast()) );
	
	m_tcpServer = new QTcpServer(this);
	connect( m_tcpServer, SIGNAL(newConnection()), this, SLOT(NewIncomingConnection()) );
}

drwNetworkServer::~drwNetworkServer()
{
    // delete all connections
    Stop();

	delete m_broadcastSocket;
	delete m_broadcastTimer;
	delete m_tcpServer;
}

void drwNetworkServer::Start()
{
	// Broadcast a first datagram and setup a timer to broadcast at regular intervals
	m_broadcastTimer->start( BroadcastTimeout );
	Broadcast();
		
	// Setup a TCP server to listen for new connections initiated by peers who received our broadcast message
	m_tcpServer->listen( QHostAddress::Any, ConnectionPort );
	
	m_isOn = true;
}

void drwNetworkServer::Stop()
{
	if( m_isOn )
	{
		m_broadcastTimer->stop();
		m_tcpServer->close();
		m_broadcastSocket->close();

		// delete all connections
        ConnectionsContainer::iterator it = m_connections.begin();
		while ( it != m_connections.end() )
		{
            drwNetworkConnection * connection = it.key();

            // needed before deleting, otherwise the connection lost callback is called.
            disconnect( connection, SIGNAL(ConnectionLost(drwNetworkConnection*)), this, SLOT(ConnectionLost(drwNetworkConnection*)) );
            disconnect( connection, SIGNAL(ConnectionReady(drwNetworkConnection*)), this, SLOT(ConnectionReadySlot(drwNetworkConnection*)));
            disconnect( connection, SIGNAL(CommandReceived(drwCommand::s_ptr)), this, SLOT(CommandReceivedSlot(drwCommand::s_ptr)) );

            delete connection;
			++it;
		}
		m_connections.clear();

		m_isOn = false;
	}
}

void drwNetworkServer::GetConnectionUserNamesAndIps( QStringList & userNameList, QStringList & ipList )
{
	ConnectionsContainer::iterator it = m_connections.begin();
	while( it != m_connections.end() )
	{
		drwNetworkConnection * connection = it.key();
		userNameList.push_back( connection->GetPeerUserName() );
		ipList.push_back( connection->GetPeerAddress().toString() );
		++it;
	}
}

void drwNetworkServer::Broadcast()
{
	QByteArray datagram = ProgramSignature + "=" + m_userName.toUtf8();
    m_broadcastSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, BroadcastPort);
}

void drwNetworkServer::NewIncomingConnection()
{
	while( m_tcpServer->hasPendingConnections() )
	{
		QTcpSocket * sock = m_tcpServer->nextPendingConnection();
        drwNetworkConnection * connection = new drwNetworkConnection( sock, this );
		connect( connection, SIGNAL(ConnectionLost(drwNetworkConnection*)), this, SLOT(ConnectionLost(drwNetworkConnection*)) );
		connect( connection, SIGNAL(ConnectionReady(drwNetworkConnection*)), this, SLOT(ConnectionReadySlot(drwNetworkConnection*)));
		connect( connection, SIGNAL(CommandReceived(drwCommand::s_ptr)), this, SLOT(CommandReceivedSlot(drwCommand::s_ptr)), Qt::DirectConnection );

		int connectionId = m_dispatcher->RequestNewUserId();
		m_connections[ connection ] = connectionId;
	}
}

void drwNetworkServer::ConnectionReadySlot( drwNetworkConnection * connection )
{
	// Send the initial command
	drwCommandDatabase * db = m_dispatcher->GetDb();
    drwCommand::s_ptr command( new drwServerInitialCommand( db->GetNumberOfCommands() ) );
	connection->SendCommand( command );

	// Send all commands in the database.
    db->LockDb( true );
    int nbCommands = db->GetNumberOfCommands();
    for( int i = 0; i < nbCommands; ++i )
    {
        // Get a copy of the next command
        drwCommand::s_ptr com = db->GetCommand( i );

        // Translate local user id if needed
        int commandId = com->GetUserId();
        if( commandId == m_dispatcher->GetLocalUserId() )
            com->SetUserId( m_translatedLocalUserId );

        // Send
        connection->SendCommand( com );
    }
    db->LockDb( false );
}

void drwNetworkServer::ConnectionLost( drwNetworkConnection * connection )
{
	ConnectionsContainer::iterator itErase = m_connections.find( connection );
	Q_ASSERT( itErase != m_connections.end() );

	m_connections.erase( itErase );
	connection->deleteLater();
}

void drwNetworkServer::CommandReceivedSlot( drwCommand::s_ptr com )
{
	drwNetworkConnection * commandSender = dynamic_cast<drwNetworkConnection*>(sender());
	Q_ASSERT( commandSender );

	// Assign a UserId to the command
	int userId = m_connections[ commandSender ];
	com->SetUserId( userId );

	// Send command to all other peers
	ConnectionsContainer::iterator it = m_connections.begin();
	while( it != m_connections.end() )
	{
		drwNetworkConnection * connection = it.key();
		if( connection != commandSender )
		{
			connection->SendCommand( com );
		}
		++it;
	}
	
	// execute it here
	m_dispatcher->IncomingNetCommand( com );
}

// Local command, must be sent to all connected clients
void drwNetworkServer::SendCommand( drwCommand::s_ptr command )
{
	// Translate local command Ids
	int commandId = command->GetUserId();
	if( commandId == m_dispatcher->GetLocalUserId() )
		command->SetUserId( m_translatedLocalUserId );
	
	// Send the command to all connections
	ConnectionsContainer::iterator it = m_connections.begin();
	while( it != m_connections.end() )
	{
		drwNetworkConnection * connection = it.key();
		connection->SendCommand( command );
		++it;
	}
}

