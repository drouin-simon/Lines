#ifndef __drwNetworkServerFinder_h_
#define __drwNetworkServerFinder_h_

#include <QObject>
#include <QStringList>
#include <QHostAddress>

class QUdpSocket;

class drwNetworkServerFinder : public QObject
{
	
	Q_OBJECT
	
public:
	
	drwNetworkServerFinder();
	~drwNetworkServerFinder();
	
	const QStringList & GetServerUsers() { return m_serverUsers; }
	const QList<QHostAddress> &  GetServerAddresses() { return m_serverAddresses; }
	
signals:
	
	void ModifiedSignal();
	
private slots:

	void ProcessReceivedBroadcastMessages();
	
protected:
	
	void TryAddingServer( QString & user, QHostAddress & address );
	
	QUdpSocket * m_broadcastSocket;
	
	QStringList m_serverUsers;
	QList<QHostAddress> m_serverAddresses;
	
};


#endif