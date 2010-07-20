#include "drwNetworkThread.h"
#include "drwNetworkInterface.h"

void drwInThreadAgent::NewCommandsToSend()
{
	m_thread->m_queueMutex.lock();
	drwNetworkThread::CommandContainer::iterator it = m_thread->m_queuedCommands.begin();
	while( it != m_thread->m_queuedCommands.end() )
	{
		m_thread->m_interface->SendCommand( *it );
		++it;
	}
	m_thread->m_queuedCommands.clear();
	m_thread->m_queueMutex.unlock();
}

drwNetworkThread::drwNetworkThread( Scene * scene, drwCommandDatabase * db, QObject * parent )
: QThread( parent )
, m_interface( 0 )
, m_scene( scene )
, m_commandDb( db )
, m_inThread( 0 )
{
}

drwNetworkThread::~drwNetworkThread()
{
	if( isRunning() )
		quit();
}

void drwNetworkThread::StartNetwork()
{
	start();
	if( isRunning() )
		emit ModifiedSignal();
}

void drwNetworkThread::StopNetwork()
{
	if( isRunning() )
	{
		quit();
		emit ModifiedSignal();
	}
}

void drwNetworkThread::SetUserName( QString userName )
{
	if(!isRunning())
	{
		m_userName = userName;
		emit ModifiedSignal();
	}
}
										  
void drwNetworkThread::GetConnectionUserNamesAndIps( QStringList & userNameList, QStringList & ipList )
{
	if( m_interface )
		m_interface->GetConnectionUserNamesAndIps( userNameList, ipList );
}

void drwNetworkThread::SendCommand( drwCommand::s_ptr command )
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

void drwNetworkThread::InterfaceModified()
{
	emit ModifiedSignal();
}

void drwNetworkThread::run()
{
	m_interface = new drwNetworkInterface( m_scene, m_commandDb );
	connect( m_interface, SIGNAL(ModifiedSignal()), this, SLOT(InterfaceModified()), Qt::QueuedConnection );
	
	m_inThread = new drwInThreadAgent( this );
	connect( this, SIGNAL(NewCommandsToSendSignal()), m_inThread, SLOT(NewCommandsToSend()), Qt::QueuedConnection );
	
	m_interface->SetUserName( m_userName );
	m_interface->Start( true, true );
	
	// Start the thread's event loop
	exec();
	
	m_inThread->deleteLater();
	m_inThread = 0;
	
	delete m_interface;
	m_interface = 0;
}
