#include "drwNetworkServerFinder.h"
#include "drwNetworkConnection.h"
#include <QUdpSocket>

drwNetworkServerFinder::drwNetworkServerFinder()
{
	m_broadcastSocket = new QUdpSocket(this);
	m_broadcastSocket->bind( BroadcastPort );
	connect(m_broadcastSocket, SIGNAL(readyRead()), this, SLOT(ProcessReceivedBroadcastMessages()));
}

drwNetworkServerFinder::~drwNetworkServerFinder()
{
	delete m_broadcastSocket;
}

void drwNetworkServerFinder::ProcessReceivedBroadcastMessages()
{
    while (m_broadcastSocket->hasPendingDatagrams()) 
	{
		// Read a datagram
        QByteArray datagram;
        datagram.resize(m_broadcastSocket->pendingDatagramSize());
		QHostAddress address;
		quint16 port;
        m_broadcastSocket->readDatagram(datagram.data(), datagram.size(), &address, &port);
		
		// Verify datagram signature and extract peer username
		QList<QByteArray> dataTokens = datagram.split('=');
		if( dataTokens.at(0) == ProgramSignature )
		{
			QString peerUserName = dataTokens.at(1);
			TryAddingServer( peerUserName, address );
		}
    }
}

void drwNetworkServerFinder::TryAddingServer( QString & user, QHostAddress & address )
{
	for( int i = 0; i < m_serverUsers.size(); ++i )
	{
		if( m_serverUsers.at( i ) == user && m_serverAddresses.at(i) == address )
			return;
	}
	
	// Not found -> add it
	m_serverUsers.push_back( user );
	m_serverAddresses.push_back( address );
	emit ModifiedSignal();
}
