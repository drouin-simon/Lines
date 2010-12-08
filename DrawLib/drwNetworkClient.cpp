#include <QtNetwork>
#include "drwNetworkClient.h"
#include "drwCommandDispatcher.h"
#include "drwNetworkConnection.h"


drwNetworkClient::drwNetworkClient( QString & peerUserName, QHostAddress & peerAddress, QObject * parent ) 
: QObject(parent)
, m_peerUserName( peerUserName )
, m_peerAddress( peerAddress )
, m_connection(0)
, m_timerId(-1)
{
}

drwNetworkClient::~drwNetworkClient()
{
	if( m_connection )
		Disconnect();
}

void drwNetworkClient::Connect( )
{
	// Create a new connection
	m_connection = new drwNetworkConnection( m_peerUserName, m_peerAddress, this );
	connect( m_connection, SIGNAL(ConnectionReady(drwNetworkConnection*)), this, SLOT(ConnectionReady(drwNetworkConnection*)) );
	connect( m_connection, SIGNAL(ConnectionLost(drwNetworkConnection*)), this, SLOT(ConnectionLost(drwNetworkConnection*)) );
	m_timerId = startTimer( 3000 );
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
	killTimer( m_timerId );
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

void drwNetworkClient::timerEvent( QTimerEvent * e )
{
	killTimer( m_timerId );
	emit ConnectionTimeoutSignal();
}
