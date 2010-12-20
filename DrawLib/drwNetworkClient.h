#ifndef __drwNetworkClient_h_
#define __drwNetworkClient_h_

#include <QObject>
#include <QHostAddress>
#include "drwCommand.h"

class drwCommandDispatcher;
class drwNetworkConnection;

class drwNetworkClient : public QObject
{
	
	Q_OBJECT
	
public:

	enum ClientState{ Init, WaitingForServer, WaitingForNbCommands, ReceivingScene, Operating, ConnectionLost, ConnectionTimedOut };
	
	drwNetworkClient( QString & userName, QHostAddress & address, drwCommandDispatcher * dispatcher );
	~drwNetworkClient();

	double GetPercentRead();

	ClientState GetState() { return m_state; }
	
	void Connect();
	void Disconnect();
	
signals:
	
	void StateChanged();
	
public slots:
	
	void SendCommand( drwCommand::s_ptr command );
	
private slots:
		
	void ConnectionReadySlot( drwNetworkConnection * connection );
	void ConnectionLostSlot( drwNetworkConnection * connection );
	void CommandReceivedSlot( drwCommand::s_ptr );
	
protected:

	void SetState( ClientState state );
	void timerEvent( QTimerEvent * e );
	
	drwNetworkConnection * m_connection;
	drwCommandDispatcher * m_dispatcher;
	
	QString m_userName;
	QString m_peerUserName;
	QHostAddress m_peerAddress;

	int m_numberOfCommandsToRead;
	int m_totalNumberOfCommandsToRead;

	ClientState m_state;

	int m_timerId; // timer used track failed connections
};

#endif
