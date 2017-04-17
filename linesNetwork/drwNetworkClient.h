#ifndef __drwNetworkClient_h_
#define __drwNetworkClient_h_

#include <QObject>
#include <QHostAddress>
#include "drwCommand.h"

class LinesCore;
class drwNetworkConnection;

class drwNetworkClient : public QObject
{
	
	Q_OBJECT
	
public:

	enum ClientState{ Init, WaitingForServer, WaitingForNbCommands, ReceivingScene, Operating, ConnectionLost, ConnectionTimedOut };
	
    drwNetworkClient( QString & userName, QHostAddress & address, LinesCore * lc );
	~drwNetworkClient();

	int GetPercentRead();
    QString GetErrorMessage() { return m_errorMessage; }

	ClientState GetState() { return m_state; }
    QString GetPeerUserName() { return m_peerUserName; }

	void Connect();
	void Disconnect();
	
signals:
	
	void StateChanged();
	
public slots:
	
	void SendCommand( drwCommand::s_ptr command );
	
private slots:
		
	void ConnectionReadySlot( drwNetworkConnection * connection );
    void ConnectionLostSlot( drwNetworkConnection * connection, QString errorMessage );
	void CommandReceivedSlot( drwCommand::s_ptr );
	
protected:

	void SetState( ClientState state );
	void timerEvent( QTimerEvent * e );
	
	drwNetworkConnection * m_connection;
    LinesCore * m_lines;
	
	QString m_userName;
	QString m_peerUserName;
	QHostAddress m_peerAddress;

	int m_numberOfCommandsToRead;
	int m_totalNumberOfCommandsToRead;

	ClientState m_state;
    QString m_errorMessage;

	int m_timerId; // timer used track failed connections
};

#endif
