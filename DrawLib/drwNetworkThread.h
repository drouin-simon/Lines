#ifndef __drwNetworkThread_h_
#define __drwNetworkThread_h_

#include <QThread>
#include <QMutex>
#include <list>
#include "drwCommand.h"

class drwNetworkInterface;
class Scene;
class drwCommandDatabase;

class drwNetworkThread;

// This class is instanciated inside the thread to facilitate
// the management of signals/slots across the thread boundary
class drwInThreadAgent : public QObject
{
	Q_OBJECT
	
public:
	
	drwInThreadAgent( drwNetworkThread * thread ) : m_thread(thread) {}
	
public slots:
	
	void NewCommandsToSend();

private:
	
	drwNetworkThread * m_thread;
};


class drwNetworkThread : public QThread
{
	
	Q_OBJECT
	
public:
	
	friend class drwInThreadAgent;
	
	drwNetworkThread( Scene * scene, drwCommandDatabase * db, QObject * parent );
	~drwNetworkThread();
	
	void StartNetwork();
	void StopNetwork();
	void SetUserName( QString userName );
	QString GetUserName() { return m_userName; }
	void GetConnectionUserNamesAndIps( QStringList & userNameList, QStringList & ipList );
	
public slots:

	void SendCommand( drwCommand::s_ptr command );		// This is called by clients outside the thread
	
signals:
	
	void NewCommandsToSendSignal();
	void ModifiedSignal();  // This is basically to relay the network interface's Modified signal.
	
private slots:
	
	void InterfaceModified();
	
private:
	
	// Function called when thread starts
	void run();
	
	QMutex m_queueMutex;
	typedef std::list<drwCommand::s_ptr> CommandContainer;
	CommandContainer m_queuedCommands;
	QString m_userName;
	drwNetworkInterface * m_interface;  // this object belongs to the thread spawned in run()
	Scene * m_scene;
	drwCommandDatabase * m_commandDb;
	drwInThreadAgent * m_inThread;
};

#endif
