#ifndef __drwNetworkManager_h_
#define __drwNetworkManager_h_

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QHostAddress>

#include "drwCommand.h"

class drwNetworkClient;
class drwNetworkServer;
class QWidget;

class drwInThreadAgent;
class drwNetState;

class drwNetworkManager : public QThread
{
	
	Q_OBJECT
	
public:

	enum MessageToThread{ ConnectMsg, DisconnectMsg, StartSharingMsg, StopSharingMsg, StartSendingMsg, FinishSendingMsg, ResetMsg };
	enum MessageFromThread{ ConnectionTimedOutMsg, ConnectionLostMsg, ConnectedMsg, StartReceivingSceneMsg, FinishReceivingSceneMsg, SendSceneMsg };
	enum NetworkState{ Idle, WaitingForConnection, ConnectionTimedOut, ConnectionLost, ReceivingScene, Connected, Sharing, SendingScene };
	
	drwNetworkManager();
	~drwNetworkManager();

	NetworkState GetState() { return m_state; }

	bool IsSharing();
	void StartSharing();
	void StopSharing();
	void StartSendingScene();
	void FinishSendingScene();
	
	bool IsConnected();
    void Connect( QString username, QHostAddress ip );

    friend class drwInThreadAgent;
	
public slots:

	void SendCommand( drwCommand::s_ptr );

private slots:

	void CommandReceivedSlot( drwCommand::s_ptr );
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

public slots:
	
	void NewCommandsToSend();
	void MessageToThreadSlot( drwNetworkManager::MessageToThread message );

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
