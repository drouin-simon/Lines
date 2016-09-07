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

QString drwNetworkManager::GetErrorMessage()
{
    return m_inThread->GetErrorMessage();
}

void drwNetworkManager::ResetState()
{
    SetState( Idle );
    m_inThread->SetErrorMessage("");  // reset error message
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
		m_inThread->SetMessageToThread( StartSharingMsg );
		emit MessageToThreadSignal();
		SetState( Sharing );
	}
}

void drwNetworkManager::StopSharing()
{
	if( IsSharing() )
	{
		m_inThread->SetMessageToThread( StopSharingMsg );
		emit MessageToThreadSignal();
		SetState( Idle );
	}
}

bool drwNetworkManager::IsConnected()
{
	return( m_state == WaitingForConnection ||
            m_state == ConnectionTimedOut ||
			m_state == Connected );
}

void drwNetworkManager::Connect( QString username, QHostAddress ip )
{
	if( m_state == Idle )
	{
		m_inThread->SetConnectAttributes( username, ip );
		m_inThread->SetMessageToThread( ConnectMsg );
		emit MessageToThreadSignal();
        SetState( WaitingForConnection );
	}
}

void drwNetworkManager::Disconnect()
{
    if( m_state == Connected )
    {
        m_inThread->SetMessageToThread( DisconnectMsg );
        emit MessageToThreadSignal();
    }
}

QString drwNetworkManager::GetServerUserName()
{
    return m_inThread->GetServerUserName();
}

void drwNetworkManager::MessageFromThreadSlot()
{
	if( m_messageFromThread == ConnectionTimedOutMsg )
		SetState( ConnectionTimedOut );
	if( m_messageFromThread == ConnectionLostMsg )
		SetState( ConnectionLost );
	if( m_messageFromThread == ReceivingSceneMsg )
		SetState( ReceivingScene );
	if( m_messageFromThread == ConnectedMsg )
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
	connect( this, SIGNAL(MessageToThreadSignal()), m_inThread, SLOT(MessageToThreadSlot()), Qt::QueuedConnection );
	connect( m_inThread, SIGNAL(MessageFromThreadSignal()), this, SLOT(MessageFromThreadSlot()), Qt::QueuedConnection );

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

QString drwInThreadAgent::GetErrorMessage()
{
    m_attributesMutex.lock();
    QString ret = m_errorMessage;
    m_attributesMutex.unlock();
    return ret;
}

void drwInThreadAgent::SetErrorMessage( QString msg )
{
    m_attributesMutex.lock();
    m_errorMessage = msg;
    m_attributesMutex.unlock();
}

QString drwInThreadAgent::GetServerUserName()
{
    if( m_client )
        return m_client->GetPeerUserName();
    return QString("");
}

int drwInThreadAgent::GetPercentRead()
{
	if( m_client )
	{
		return m_client->GetPercentRead();
	}
	return 0;
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

void drwInThreadAgent::MessageToThreadSlot()
{
	if( m_messageToThread == drwNetworkManager::StartSharingMsg )
	{
		if( !m_server )
		{
			m_server = new drwNetworkServer( m_manager->m_dispatcher );
			m_server->Start();
		}
	}
	else if( m_messageToThread == drwNetworkManager::StopSharingMsg )
	{
		Reset();
	}
	else if( m_messageToThread == drwNetworkManager::ConnectMsg )
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
	else if( m_messageToThread == drwNetworkManager::DisconnectMsg )
	{
		Reset();
        m_manager->SetMessageFromThread( drwNetworkManager::ConnectionLostMsg );
        emit MessageFromThreadSignal();
	}
	else if( m_messageToThread == drwNetworkManager::ResetMsg )
	{
		Reset();
	}
}

void drwInThreadAgent::ClientStateChanged()
{
	drwNetworkClient::ClientState state = m_client->GetState();
	if( state == drwNetworkClient::ConnectionTimedOut )
	{
		m_manager->SetMessageFromThread( drwNetworkManager::ConnectionTimedOutMsg );
		emit MessageFromThreadSignal();
	}
	else if( state == drwNetworkClient::ConnectionLost )
	{
        SetErrorMessage( m_client->GetErrorMessage() );
        Reset();
		m_manager->SetMessageFromThread( drwNetworkManager::ConnectionLostMsg );
		emit MessageFromThreadSignal();
	}
	else if( state == drwNetworkClient::WaitingForServer ||
			 state == drwNetworkClient::WaitingForNbCommands ||
			 state == drwNetworkClient::ReceivingScene )
	{
		m_manager->SetMessageFromThread( drwNetworkManager::ReceivingSceneMsg );
		emit MessageFromThreadSignal();
	}
	else if( state == drwNetworkClient::Operating )
	{
		m_manager->SetMessageFromThread( drwNetworkManager::ConnectedMsg );
		emit MessageFromThreadSignal();
	}
}

void drwInThreadAgent::Reset()
{
	if( m_client )
	{
        disconnect( m_client, SIGNAL(StateChanged()), this, SLOT(ClientStateChanged()) );
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
