#ifndef __drwNetworkClient_h_
#define __drwNetworkClient_h_

#include <QObject>
#include <QHostAddress>
#include "drwCommand.h"


class drwNetworkConnection;

class drwNetworkClient : public QObject
{
	
	Q_OBJECT
	
public:
	
	drwNetworkClient( QString & userName, QHostAddress & address, QObject * parent = 0 );
	~drwNetworkClient();
	
	void Connect();
	void Disconnect();
	
signals:
	
	void ModifiedSignal();
	void CommandReceivedSignal( drwCommand::s_ptr );
	
public slots:
	
	void SendCommand( drwCommand::s_ptr command );
	void ConnectionReady( drwNetworkConnection * connection );
	void ConnectionLost( drwNetworkConnection * connection );
	
private slots:
		
	void CommandReceivedSlot( drwCommand::s_ptr );
	
protected:
	
	drwNetworkConnection * m_connection;
	
	QString m_userName;
	QString m_peerUserName;
	QHostAddress m_peerAddress;
};

#endif
