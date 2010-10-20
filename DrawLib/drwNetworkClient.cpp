#include <QtNetwork>
#include "drwNetworkClient.h"
#include "drwCommandDispatcher.h"
#include "drwNetworkConnection.h"


drwNetworkClient::drwNetworkClient( drwCommandDispatcher * dispatcher, QObject * parent ) 
: QObject(parent)
, m_dispatcher( dispatcher )
, m_connection(0)
{
}

void drwNetworkClient::Connect( QString & peerUserName, QHostAddress & peerAddress )
{
	// Create a new connection
	m_connection = new drwNetworkConnection( m_userName, peerUserName, peerAddress, this );
	connect( m_connection, SIGNAL(ConnectionReady(drwNetworkConnection*)), this, SLOT(ConnectionReady(drwNetworkConnection*)) );
	connect( m_connection, SIGNAL(ConnectionLost(drwNetworkConnection*)), this, SLOT(ConnectionLost(drwNetworkConnection*)) );
	emit ModifiedSignal();
}

void drwNetworkClient::Disconnect()
{
	if( m_connection )
	{
		m_connection->Disconnect();
		delete m_connection;
		m_connection = 0;
		emit ModifiedSignal();
	}
}

void drwNetworkClient::ConnectionReady( drwNetworkConnection * connection )
{
	connect( connection, SIGNAL(CommandReceived(drwCommand::s_ptr)), this, SLOT(CommandReceivedSlot( drwCommand::s_ptr )), Qt::DirectConnection );
	emit ModifiedSignal();
}

void drwNetworkClient::ConnectionLost( drwNetworkConnection * connection )
{
	m_connection->deleteLater();
	m_connection = 0;
	emit ModifiedSignal();
}

void drwNetworkClient::SendCommand( drwCommand::s_ptr command )
{
	m_connection->SendCommand( command );
}

void drwNetworkClient::CommandReceivedSlot( drwCommand::s_ptr command )
{
	emit CommandReceivedSignal( command );
}
