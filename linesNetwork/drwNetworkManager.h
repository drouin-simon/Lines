#ifndef __drwNetworkManager_h_
#define __drwNetworkManager_h_

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QHostAddress>

#include "drwCommand.h"
#include "drwRemoteCommandIO.h"

class drwCommandDispatcher;
class drwNetworkClient;
class drwNetworkServer;
class QWidget;

class drwInThreadAgent;
class drwNetState;

class drwNetworkManager : public QThread, public drwRemoteCommandIO
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
    QString GetErrorMessage();
    void ResetState();
	double GetPercentRead();

	bool IsSharing();
	void StartSharing();
	void StopSharing();
	
	bool IsConnected();
    void Connect( QString username, QHostAddress ip );
    void Disconnect();
    QString GetServerUserName();

	void SetMessageFromThread( MessageFromThread msg ) { m_messageFromThread = msg; }
	friend class drwInThreadAgent;

    // Implements drwRemoteCommandIO interface
	void SendCommand( drwCommand::s_ptr );

private slots:

	void MessageFromThreadSlot();
	
signals:
	
	// signals for clients
	void StateChangedSignal();

	// Signals used internally
	void MessageToThreadSignal();
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
	MessageFromThread m_messageFromThread;

	NetworkState m_state;
};


// This class is instanciated inside the thread to facilitate
// the management of signals/slots across the thread boundary
class drwInThreadAgent : public QObject
{
	Q_OBJECT
	
public:
	
	drwInThreadAgent( drwNetworkManager * man )
		: m_manager(man), m_client(0), m_server(0) {}
	
	void SetConnectAttributes( QString user, QHostAddress remoteIp );
    QString GetServerUserName();
	int GetPercentRead();

	void SetMessageToThread( drwNetworkManager::MessageToThread msg ) { m_messageToThread = msg; }

    QString GetErrorMessage();
    void SetErrorMessage( QString msg );

public slots:
	
	void NewCommandsToSend();
	void MessageToThreadSlot();

private slots:

	void ClientStateChanged();

signals:

	void MessageFromThreadSignal();
	
private:

	void Reset();
	
	drwNetworkClient * m_client;
	QMutex m_attributesMutex;
	QString m_userName;
	QHostAddress m_remoteIp;
    QString m_errorMessage;

	drwNetworkServer * m_server;
	drwNetworkManager * m_manager;

	drwNetworkManager::MessageToThread m_messageToThread;
};

#endif
