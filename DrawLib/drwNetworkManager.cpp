#include "drwNetworkManager.h"
#include "drwNetworkServer.h"
#include "drwNetworkClient.h"
#include "drwCommand.h"

drwNetworkManager::drwNetworkManager()
	: m_dispatcher(0)
	, m_inThread(0)
	, m_state( Idle )
{
	// Start the thread
	start();
}

drwNetworkManager::~drwNetworkManager()
{
	// tell the thread to quit
	quit();

	// give the thread a chance to quit by itself
	bool success = wait( 3000 );
	if( !success )
	{
		terminate();
		delete m_inThread;
	}
}

double drwNetworkManager::GetPercentRead()
{
	if( m_state == ReceivingScene )
	{
		return m_inThread->GetPercentRead();
	}
	return 0.0;
}

bool drwNetworkManager::IsSharing()
{
	return ( m_state == Sharing );
}

void drwNetworkManager::StartSharing()
{
	if( m_state == Idle )
	{
		emit MessageToThreadSignal( StartSharingMsg );
		SetState( Sharing );
	}
}

void drwNetworkManager::StopSharing()
{
	if( IsSharing() )
	{
		emit MessageToThreadSignal( StopSharingMsg );
		SetState( Idle );
	}
}

bool drwNetworkManager::IsConnected()
{
	return( m_state == WaitingForConnection ||
			m_state == ConnectionTimedOut ||
			m_state == ConnectionLost ||
			m_state == Connected );
}

void drwNetworkManager::Connect( QString username, QHostAddress ip )
{
	if( m_state == Idle )
	{
		m_inThread->SetConnectAttributes( username, ip );
		emit MessageToThreadSignal( ConnectMsg );
		m_state = WaitingForConnection;
	}
}

void drwNetworkManager::MessageFromThreadSlot( MessageFromThread message )
{
	if( message == ConnectionTimedOutMsg )
		SetState( ConnectionTimedOut );
	if( message == ConnectionLostMsg )
		SetState( ConnectionLost );
	if( message == ReceivingSceneMsg )
		SetState( ReceivingScene );
	if( message == ConnectedMsg )
		SetState( Connected );
}

void drwNetworkManager::SendCommand( drwCommand::s_ptr command )
{
	if( IsSharing() || IsConnected() )
	{
		// Copy the command
		drwCommand::s_ptr commandCopy = command->Clone();
	
		// Put it on the stack
		m_queueMutex.lock();
		m_queuedCommands.push_back( commandCopy );
		m_queueMutex.unlock();
	
		// Tell the thread that new commands are available
		emit NewCommandsToSendSignal();
	}
}

void drwNetworkManager::run()
{
	m_inThread = new drwInThreadAgent( this );
	connect( this, SIGNAL(NewCommandsToSendSignal()), m_inThread, SLOT(NewCommandsToSend()), Qt::QueuedConnection );
	connect( this, SIGNAL(MessageToThreadSignal(MessageToThread)), m_inThread, SLOT(MessageToThreadSlot(drwNetworkManager::MessageToThread)));
	connect( m_inThread, SIGNAL(MessageFromThread(drwNetworkManager::MessageFromThread)), this, SLOT(MessageFromThreadSlot(MessageFromThread)));

	// Start the thread's event loop
	exec();
	
	m_inThread->deleteLater();
	m_inThread = 0;
}

void drwNetworkManager::SetState( NetworkState state )
{
	if( state != m_state )
	{
		m_state = state;
		emit StateChangedSignal();
	}
}

void drwInThreadAgent::SetConnectAttributes( QString user, QHostAddress remoteIp )
{
	m_attributesMutex.lock();
	m_userName = user;
	m_remoteIp = remoteIp;
	m_attributesMutex.unlock();
}

double drwInThreadAgent::GetPercentRead()
{
	if( m_client )
	{
		return m_client->GetPercentRead();
	}
	return 0.0;
}

void drwInThreadAgent::NewCommandsToSend()
{
	m_manager->m_queueMutex.lock();
	drwNetworkManager::CommandContainer::iterator it = m_manager->m_queuedCommands.begin();
	while( it != m_manager->m_queuedCommands.end() )
    {
		if( m_client )
			m_client->SendCommand( *it );
		else if( m_server )
			m_server->SendCommand( *it );
        ++it;
    }
	m_manager->m_queuedCommands.clear();
	m_manager->m_queueMutex.unlock();
}

void drwInThreadAgent::MessageToThreadSlot( drwNetworkManager::MessageToThread message )
{
	if( message == drwNetworkManager::StartSharingMsg )
	{
		if( !m_server )
		{
			m_server = new drwNetworkServer( m_manager->m_dispatcher );
			m_server->Start();
		}
	}
	else if( message == drwNetworkManager::StopSharingMsg )
	{
		Reset();
	}
	else if( message == drwNetworkManager::ConnectMsg )
	{
		if( !m_client )
		{
			m_attributesMutex.lock();
			m_client = new drwNetworkClient( m_userName, m_remoteIp, m_manager->m_dispatcher );
			m_attributesMutex.unlock();
			connect( m_client, SIGNAL(StateChanged()), this, SLOT(ClientStateChanged()) );
			m_client->Connect();
		}
	}
	else if( message == drwNetworkManager::DisconnectMsg )
	{
		Reset();
	}
	else if( message == drwNetworkManager::ResetMsg )
	{
		Reset();
	}
}

void drwInThreadAgent::ClientStateChanged()
{
	drwNetworkClient::ClientState state = m_client->GetState();
	if( state == drwNetworkClient::ConnectionTimedOut )
		emit MessageFromThread( drwNetworkManager::ConnectionTimedOutMsg );
	else if( state == drwNetworkClient::ConnectionLost )
		emit MessageFromThread( drwNetworkManager::ConnectionLostMsg );
	else if( state == drwNetworkClient::WaitingForServer ||
			 state == drwNetworkClient::WaitingForNbCommands ||
			 state == drwNetworkClient::ReceivingScene )
		emit MessageFromThread( drwNetworkManager::ReceivingSceneMsg );
	else if( state == drwNetworkClient::Operating )
		emit MessageFromThread( drwNetworkManager::ConnectedMsg );
}

void drwInThreadAgent::Reset()
{
	if( m_client )
	{
		m_client->Disconnect();
		m_client->deleteLater();
		m_client = 0;
	}
	if( m_server )
	{
		m_server->Stop();
		m_server->deleteLater();
		m_server = 0;
	}
}
