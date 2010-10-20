#ifndef __drwNetworkClient_h_
#define __drwNetworkClient_h_

#include <QObject>
#include "drwCommand.h"

class QHostAddress;

class drwNetworkConnection;
class drwCommandDispatcher;

class drwNetworkClient : public QObject
{
	
	Q_OBJECT
	
public:
	
	drwNetworkClient( drwCommandDispatcher * dispatcher, QObject * parent = 0 );
	~drwNetworkClient();
	
	void Connect( QString & userName, QHostAddress & address );
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
	
	drwCommandDispatcher * m_dispatcher;
	drwNetworkConnection * m_connection;
	
	QString m_userName;
	
};

#endif
