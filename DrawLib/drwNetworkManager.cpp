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

drwNetworkManager::~drwNetworkManager()
{
	if( isRunning() )
		quit();
}

bool drwNetworkManager::IsSharing()
{
	if( m_server )
		return true;
	return false;
}

void drwNetworkManager::StartSharing()
{
	if( !IsSharing() && !IsConnected() )
	{
        m_server = new drwNetworkServer();
		start();
		if( isRunning() )
			emit ModifiedSignal();
	}
}

void drwNetworkManager::StopSharing()
{
	if( isRunning() )
	{
		quit();
		m_server->deleteLater();
		emit ModifiedSignal();
	}
}

bool drwNetworkManager::IsConnected()
{
	if( m_client )
		return true;
	return false;
}


void drwNetworkManager::Connect( QString username, QHostAddress ip )
{
	if( !IsSharing() && !IsConnected() )
	{
        m_client = new drwNetworkClient( username, ip );
		start();
		if( isRunning() )
			emit ModifiedSignal();
	}
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
	
	if( m_server )
	{
		m_server->Start();
	}
	else if( m_client )
	{
		m_client->Connect();
	}
	
	// Start the thread's event loop
	exec();
	
	m_inThread->deleteLater();
	m_inThread = 0;
	
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
