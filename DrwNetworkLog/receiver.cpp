#include <QtGui>
#include <QtNetwork>

#include "receiver.h"
#include "drwNetworkInterface.h"
#include "drwNetworkConnection.h"

Receiver::Receiver(QWidget *parent) : QDialog(parent), m_connection(NULL)
{
	// Setup GUI
    statusLabel = new QLabel(tr("Listening for broadcasted messages"));
	
	textOutput = new QTextEdit(this);
	
    quitButton = new QPushButton(tr("&Quit"));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(statusLabel);
	mainLayout->addWidget(textOutput);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Broadcast Receiver"));
	
	// Setup network interface
	m_networkInterface = new drwNetworkInterface(this);
	m_networkInterface->SetUserName(QString("Receiver"));
	connect( m_networkInterface, SIGNAL(NewConnection(drwNetworkConnection *)), this, SLOT(NewConnection(drwNetworkConnection *)));
	connect( m_networkInterface, SIGNAL(ConnectionLost(drwNetworkConnection *)), this, SLOT(ConnectionLost(drwNetworkConnection *)));
	m_networkInterface->Start( false, true );
}

void Receiver::NewConnection( drwNetworkConnection * connection )
{
	m_connection = connection;
	
	statusLabel->setText(tr("Connected\nUser: %1\nIP: %2\n")
						 .arg( connection->GetPeerUserName())
						 .arg( connection->GetPeerAddress().toString() ) );
	
	connect( connection, SIGNAL(CommandReceived(drwCommand::s_ptr)), this, SLOT(CommandReceived(drwCommand::s_ptr)) );
	
	textOutput->document()->clear();
}

void Receiver::ConnectionLost( drwNetworkConnection * connection )
{
	statusLabel->setText(tr("Listening for broadcasted messages"));
	disconnect( connection, SIGNAL(CommandReceived(drwCommand::s_ptr)), this, SLOT(CommandReceived(drwCommand::s_ptr)) );
}

void Receiver::CommandReceived( drwCommand::s_ptr com )
{
	QString msg;
	QTextStream stream(&msg);
	com->Write( stream );
	stream << endl;
	textOutput->textCursor().insertText( msg );
}

