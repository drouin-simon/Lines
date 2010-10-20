#include "drwNetworkManager.h"

drwNetworkManager::drwNetworkManager( drwCommandDispatcher * dispatcher ) 
: m_client(0)
, m_server(0)
, m_dispatcher( dispatcher )
{
}

bool drwNetworkManager::IsSharing()
{
	if( m_server )
		return true;
}

void drwNetworkManager::StartSharing()
{
	if( !IsSharing() && !IsConnected() )
	{
		m_server = new drwNetworkServer( m_dispatcher );
	}
}

void drwNetworkManager::StopSharing()
{
	
}

bool drwNetworkManager::IsConnected()
{
	if( m_client )
		return true;
}

QWidget * drwNetworkManager::CreateConnectionWidget()
{
}

void drwNetworkManager::StartSharedSessionSearch()
{
	if( !IsSharing() && !IsSearching() && !IsConnected() )
	{
		m_client = new drwNetworkClient( m_dispatcher );
	}
}

void drwNetworkManager::StopSharedSessionSearch()
{
	if( IsSearching() )
	{
	}
}

void drwNetworkManager::Connect( QString username, QHostAddress ip )
{
}

void drwNetworkManager::SendCommand( drwCommand::s_ptr com )
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