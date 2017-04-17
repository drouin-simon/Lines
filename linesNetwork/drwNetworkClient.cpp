#include <QtNetwork>
#include "drwNetworkClient.h"
#include "LinesCore.h"
#include "drwNetworkConnection.h"


drwNetworkClient::drwNetworkClient( QString & peerUserName, QHostAddress & peerAddress, LinesCore * lc )
	: m_connection(0)
    , m_lines( lc )
	, m_peerUserName( peerUserName )
	, m_peerAddress( peerAddress )
	, m_numberOfCommandsToRead(0)
	, m_totalNumberOfCommandsToRead(0)
	, m_state( Init )
	, m_timerId(-1)
{
}

drwNetworkClient::~drwNetworkClient()
{
	if( m_connection )
		Disconnect();
}

int drwNetworkClient::GetPercentRead()
{
	if( m_totalNumberOfCommandsToRead != 0 )
	{
		double diff = m_totalNumberOfCommandsToRead - m_numberOfCommandsToRead;
		double frac = diff / m_totalNumberOfCommandsToRead;
		int percent = (int)round( frac * 100.0 );
		return percent;
	}
	return 0;
}

void drwNetworkClient::Connect( )
{
	// Create a new connection
	m_connection = new drwNetworkConnection( m_peerUserName, m_peerAddress );
    connect( m_connection, SIGNAL(ConnectionLost(drwNetworkConnection*,QString)), this, SLOT(ConnectionLostSlot(drwNetworkConnection*,QString)) );
	connect( m_connection, SIGNAL(ConnectionReady(drwNetworkConnection*)), this, SLOT(ConnectionReadySlot(drwNetworkConnection*)) );
	m_timerId = startTimer( 3000 );
	SetState( WaitingForServer );
}

void drwNetworkClient::Disconnect()
{
	if( m_connection )
	{
		m_connection->Disconnect();
		delete m_connection;
		m_connection = 0;
		SetState( Init );
	}
}

void drwNetworkClient::ConnectionReadySlot( drwNetworkConnection * connection )
{
	connect( connection, SIGNAL(CommandReceived(drwCommand::s_ptr)), this, SLOT(CommandReceivedSlot( drwCommand::s_ptr )), Qt::DirectConnection );
	killTimer( m_timerId );
	m_timerId = -1;
	SetState( WaitingForNbCommands );
}

void drwNetworkClient::ConnectionLostSlot( drwNetworkConnection * connection, QString errorMessage )
{
    m_errorMessage = errorMessage;
	m_connection->deleteLater();
	m_connection = 0;
    SetState( ConnectionLost );
}

void drwNetworkClient::SendCommand( drwCommand::s_ptr command )
{
	m_connection->SendCommand( command );
}

void drwNetworkClient::CommandReceivedSlot( drwCommand::s_ptr command )
{
	if( m_state == WaitingForNbCommands )
	{
		if( command->GetCommandId() == drwIdServerInitialCommand )
		{
			drwServerInitialCommand * serverMsg = dynamic_cast<drwServerInitialCommand*> (command.get());
			m_totalNumberOfCommandsToRead = serverMsg->GetNumberOfCommands();
			m_numberOfCommandsToRead = m_totalNumberOfCommandsToRead;
            if( m_numberOfCommandsToRead > 0 )
                SetState( ReceivingScene );
            else
                SetState( Operating );
            m_lines->IncomingNetCommand( command );
		}
	}
	else
	{
        m_lines->IncomingNetCommand( command );
		if( m_state == ReceivingScene )
		{
            if( m_numberOfCommandsToRead == 1 )
			{
				SetState( Operating );
			}
			m_numberOfCommandsToRead--;
		}
	}
}

void drwNetworkClient::SetState( ClientState state )
{
	if( state != m_state )
	{
		m_state = state;
		emit StateChanged();
	}
}

void drwNetworkClient::timerEvent( QTimerEvent * e )
{
	killTimer( m_timerId );
	m_timerId = 0;
	m_connection->deleteLater();
	m_connection = 0;
	SetState( ConnectionTimedOut );
}
