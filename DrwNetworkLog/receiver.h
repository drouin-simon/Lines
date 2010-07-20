#ifndef __Receiver_h_
#define __Receiver_h_

#include <QDialog>
#include "drwCommand.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QTextEdit;
class QVBoxLayout;
QT_END_NAMESPACE
class drwNetworkConnection;
class drwNetworkInterface;
class drwCommand;

class Receiver : public QDialog
{
    Q_OBJECT

public:
    Receiver(QWidget *parent = 0);

private slots:
	void CommandReceived(drwCommand::s_ptr);
	void NewConnection(drwNetworkConnection *);
	void ConnectionLost(drwNetworkConnection *);

private:
    QLabel *statusLabel;
	QTextEdit * textOutput;
    QPushButton *quitButton;
	QVBoxLayout *mainLayout;
	drwNetworkInterface * m_networkInterface;
	drwNetworkConnection * m_connection;
};

#endif
