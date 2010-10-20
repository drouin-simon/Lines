#ifndef __drwNetworkManager_h_
#define __drwNetworkManager_h_

#include <QThread>
#include <QObject>
#include <QMutex>

#include "drwCommand.h"

class drwCommandDispatcher;
class drwNetworkClient;
class drwNetworkServer;
class QWidget;

class drwInThreadAgent;

class drwNetworkManager : public QThread
{
	
	Q_OBJECT
	
public:
	
	drwNetworkManager( drwCommandDispatcher * dispatcher );

	bool IsSharing();
	void StartSharing();
	void StopSharing();
	
	bool IsConnected();
	QWidget * CreateConnectionWidget();
	
public slots:

	void SendCommand( drwCommand::s_ptr );
	
signals:
	
	void NewCommandsToSendSignal();
	
protected:
	
	drwNetworkClient * m_client;
	drwNetworkServer * m_server;
	drwCommandDispatcher * m_dispatcher;
	
	// Function called when thread starts
	void run();
	
	// Queued commands to send and mutex to protect it since it is accessed by main and net threads
	QMutex m_queueMutex;
	typedef std::list<drwCommand::s_ptr> CommandContainer;
	CommandContainer m_queuedCommands;
	
	drwInThreadAgent * m_inThread;
	
};


// This class is instanciated inside the thread to facilitate
// the management of signals/slots across the thread boundary
class drwInThreadAgent : public QObject
{
	Q_OBJECT
	
public:
	
	drwInThreadAgent( drwNetworkManager * thread ) : m_thread(thread) {}
	
public slots:
	
	void NewCommandsToSend();
	
private:
	
	drwNetworkManager * m_thread;
};

#endif