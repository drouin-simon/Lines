#include "drwNetworkManager.h"
#include "drwNetworkServer.h"
#include "drwNetworkClient.h"
#include "drwCommand.h"

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
        m_server = new drwNetworkServer();
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


void drwNetworkManager::Connect( QString username, QHostAddress ip )
{
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
}

void drwInThreadAgent::NewCommandsToSend()
{
    m_thread->m_queueMutex.lock();
    drwNetworkManager::CommandContainer::iterator it = m_thread->m_queuedCommands.begin();
    while( it != m_thread->m_queuedCommands.end() )
    {
        if( m_thread->m_client )
            m_thread->m_client->SendCommand( *it );
        else if( m_thread->m_server )
            m_thread->m_server->SendCommand( *it );
        ++it;
    }
    m_thread->m_queuedCommands.clear();
    m_thread->m_queueMutex.unlock();
}
