#include "drwNetworkConnection.h"
#include <QtNetwork>

drwNetworkConnection::drwNetworkConnection( QString userName, QTcpSocket * socket, QObject * parent ) 
: QObject(parent)
, m_socket( socket )
, m_peerAddress( socket->peerAddress() )
, m_userName(userName)
, m_ready(false)
{
	m_socket->setSocketOption( QAbstractSocket::LowDelayOption, 1 );  // make sure commands are not buffered before sending
	connect( m_socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
	connect( m_socket, SIGNAL(disconnected()), this, SLOT(SocketDisconnected()) );
}

drwNetworkConnection::drwNetworkConnection( QString userName, QString peerUserName, QHostAddress & address, QObject *parent ) 
: QObject(parent)
, m_socket(0)
, m_peerUserName( peerUserName )
, m_peerAddress( address )
, m_userName( userName )
, m_ready( false )
{
	m_socket = new QTcpSocket(this);
	m_socket->setSocketOption( QAbstractSocket::LowDelayOption, 1 );  // make sure commands are not buffered before sending
	connect( m_socket, SIGNAL(connected()), this, SLOT(SocketConnected()) );
	connect( m_socket, SIGNAL(disconnected()), this, SLOT(SocketDisconnected()) );
	m_socket->connectToHost( address, ConnectionPort );
}

drwNetworkConnection::~drwNetworkConnection()
{
	Disconnect();
}

void drwNetworkConnection::Disconnect()
{
	if( m_socket )
	{
		m_socket->disconnectFromHost();
		delete m_socket;
		m_socket = 0;
	}
}

void drwNetworkConnection::SendCommand( drwCommand::s_ptr command )
{
	if( m_ready )
	{
		QDataStream stream( m_socket );
		command->Write( stream );
		m_socket->flush();
	}
}

void drwNetworkConnection::processReadyRead()
{
	if( !m_ready )
	{
		// We didn't initiate the connection, we must get the peer's username
		m_peerUserName = ReadString();
		m_ready = true;
		emit( ConnectionReady( this ) );
	}
	else 
	{
		QDataStream stream( m_socket );
		bool enoughData = true;
		while( enoughData )
		{
			int bytesAvailable = m_socket->bytesAvailable();
			if( !m_pendingCommand )
			{
				// we need to read header
				if( bytesAvailable >= drwCommand::HeaderSize() )
					m_pendingCommand = drwCommand::ReadHeader( stream );
				else 
					enoughData = false;
			}
			else 
			{
				// read the body of the command now that we know what size it is
				if( bytesAvailable >= m_pendingCommand->BodySize() )
				{
					m_pendingCommand->Read( stream );
					emit CommandReceived( m_pendingCommand );
					m_pendingCommand.reset();
				}
				else
					enoughData = false;
			}
		}
	}
}

void drwNetworkConnection::SocketConnected()
{
	m_ready = true;
	
	// Listen to what is received through the socket
	connect( m_socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()) );
	
	// Send username to peer
	SendString( m_userName );
	
	// Tell clients the connection is ready
	emit(ConnectionReady( this ));
}

void drwNetworkConnection::SocketDisconnected()
{
	emit ConnectionLost( this );
}

void drwNetworkConnection::SendString( const QString & message )
{
	QByteArray msg = message.toUtf8();
	quint16 msgLength = msg.size();
	m_socket->write( (char*)(&msgLength), sizeof(quint16) );
	m_socket->write( msg.data(), msgLength );
}

QString drwNetworkConnection::ReadString()
{
	QString res;
	quint16 stringLength;
	m_socket->read( (char*)(&stringLength), sizeof(quint16) );
	if( stringLength != 0 )
	{
		QByteArray buffer( stringLength, '\0' );
		m_socket->read( buffer.data(), stringLength );
		res = buffer;
	}
	return res;
}
