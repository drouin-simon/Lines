#include "drwNetworkConnection.h"
#include <QtNetwork>
#include <QProcessEnvironment>

// Server mode connection - socket is already created
// Server needs to read client user name before starting to send commands
drwNetworkConnection::drwNetworkConnection( QTcpSocket * socket, QObject * parent ) 
: QObject(parent)
, m_socket( socket )
, m_peerAddress( socket->peerAddress() )
, m_nextRead( drwNetworkConnection::PeerNameSize )
, m_nextReadSize( sizeof(quint16) )
{
	m_userName = ComputeUserName();
	m_socket->setSocketOption( QAbstractSocket::LowDelayOption, 1 );  // make sure commands are not buffered before sending
	connect( m_socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
	connect( m_socket, SIGNAL(disconnected()), this, SLOT(SocketDisconnected()) );
}

// Client mode connection - need to create the socket
// client sends username and starts waiting for commands as soon as the server has accepted request to connect
drwNetworkConnection::drwNetworkConnection( QString peerUserName, QHostAddress & address, QObject *parent ) 
: QObject(parent)
, m_socket(0)
, m_peerUserName( peerUserName )
, m_peerAddress( address )
, m_nextRead( drwNetworkConnection::CommandHeader )
, m_nextReadSize( sizeof(int) )
{
	m_userName = ComputeUserName();
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
	QDataStream stream( m_socket );
	command->Write( stream );
	m_socket->flush();
}

void drwNetworkConnection::processReadyRead()
{
	while( m_nextReadSize <= m_socket->bytesAvailable() )
	{
		if( m_nextRead == PeerNameSize )
		{
			quint16 stringLength;
			m_socket->read( (char*)(&stringLength), sizeof(quint16) );
			m_nextReadSize = stringLength;
			m_nextRead = PeerName;
		}
		else if( m_nextRead == PeerName )
		{
			QByteArray buffer( m_nextReadSize, '\0' );
			m_socket->read( buffer.data(), m_nextReadSize );
			m_peerUserName = buffer;
			m_nextReadSize = drwCommand::HeaderSize();
			m_nextRead = CommandHeader;
			emit ConnectionReady( this );  // server connection is ready
		}
		else if( m_nextRead == CommandHeader )
		{
			QDataStream stream( m_socket );
			m_pendingCommand = drwCommand::ReadHeader( stream );
			m_nextReadSize = m_pendingCommand->BodySize();
			m_nextRead = CommandBody;
		}
		else if( m_nextRead == CommandBody )
		{
			QDataStream stream( m_socket );
			m_pendingCommand->Read( stream );
			emit CommandReceived( m_pendingCommand );
			m_pendingCommand.reset();
			m_nextReadSize = drwCommand::HeaderSize();
			m_nextRead = CommandHeader;
		}
	}
}

// This is called when the server has accepted the connection. We then send
// our username and wait for server to send all its info
void drwNetworkConnection::SocketConnected()
{	
	// Send username to peer
	SendString( m_userName );
		
	// Listen to what is received through the socket
	connect( m_socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()) );

	emit ConnectionReady( this ); // client connection is ready
}

void drwNetworkConnection::SocketDisconnected()
{
	emit ConnectionLost( this );
}

QString drwNetworkConnection::ComputeUserName()
{
	QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
	QString userName;
	QString machineName = QHostInfo::localHostName();
	if( pe.contains( QString("USER") ) )
		userName = pe.value( QString("USER") );
	else if( pe.contains( QString("USERNAME") ) )
		userName = pe.value( QString("USERNAME") );
	QString allUserName = userName + QString("@") + machineName;
	return allUserName;
}

void drwNetworkConnection::SendString( const QString & message )
{
	QByteArray msg = message.toUtf8();
	quint16 msgLength = msg.size();
	m_socket->write( (char*)(&msgLength), sizeof(quint16) );
	m_socket->write( msg.data(), msgLength );
}

