#ifndef __drwNetworkManager_h_
#define __drwNetworkManager_h_

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QHostAddress>

#include "drwCommand.h"

class drwCommandDispatcher;
class drwNetworkClient;
class drwNetworkServer;
class QWidget;

class drwInThreadAgent;
class drwNetState;

class drwNetworkManager : public QThread
{
	
	Q_OBJECT
	
public:

	enum MessageToThread{ ConnectMsg, DisconnectMsg, StartSharingMsg, StopSharingMsg, ResetMsg };
	enum MessageFromThread{ ConnectionTimedOutMsg, ConnectionLostMsg, ReceivingSceneMsg, ConnectedMsg };
	enum NetworkState{ Idle, WaitingForConnection, ConnectionTimedOut, ConnectionLost, ReceivingScene, Connected, Sharing };
	
	drwNetworkManager();
	~drwNetworkManager();

	void SetDispatcher( drwCommandDispatcher * disp ) { m_dispatcher = disp; }
	NetworkState GetState() { return m_state; }
	double GetPercentRead();

	bool IsSharing();
	void StartSharing();
	void StopSharing();
	
	bool IsConnected();
    void Connect( QString username, QHostAddress ip );

    friend class drwInThreadAgent;
	
public slots:

	void SendCommand( drwCommand::s_ptr );

private slots:

	void MessageFromThreadSlot( MessageFromThread );
	
signals:
	
	// signals for clients
	void StateChangedSignal();
	void CommandReceivedSignal( drwCommand::s_ptr );

	// Signals used internally
	void MessageToThreadSignal( MessageToThread message );
	void NewCommandsToSendSignal();
	
protected:

	void SetState( NetworkState state );
	
	// Function called when thread starts. It is started when the class is instanciated
	void run();

	drwCommandDispatcher * m_dispatcher;
	
	// Queued commands to send and mutex to protect it since it is accessed by main and net threads
	QMutex m_queueMutex;
	typedef std::list<drwCommand::s_ptr> CommandContainer;
	CommandContainer m_queuedCommands;
	
	drwInThreadAgent * m_inThread;

	NetworkState m_state;
};


// This class is instanciated inside the thread to facilitate
// the management of signals/slots across the thread boundary
class drwInThreadAgent : public QObject
{
	Q_OBJECT
	
public:
	
	drwInThreadAgent( drwNetworkManager * man ) : m_manager(man) {}
	
	void SetConnectAttributes( QString user, QHostAddress remoteIp );
	double GetPercentRead();

public slots:
	
	void NewCommandsToSend();
	void MessageToThreadSlot( drwNetworkManager::MessageToThread message );

private slots:

	void ClientStateChanged();

signals:

	void MessageFromThread( drwNetworkManager::MessageFromThread message );
	
private:

	void Reset();
	
	drwNetworkClient * m_client;
	QMutex m_attributesMutex;
	QString m_userName;
	QHostAddress m_remoteIp;

	drwNetworkServer * m_server;
	drwNetworkManager * m_manager;
};

#endif
