#ifndef __drwCommandDispatcher_h_
#define __drwCommandDispatcher_h_

#include <QObject>
#include <QMap>
#include "drwCommand.h"

class Scene;
class drwToolbox;
class drwNetworkManager;
class drwCommandDatabase;

class drwCommandDispatcher : public QObject
{
	
	Q_OBJECT
	
public:
	
	drwCommandDispatcher( drwNetworkManager * net, drwCommandDatabase * db, drwToolbox * local, Scene * scene, QObject * parent );
	~drwCommandDispatcher();
	drwCommandDatabase * GetDb() { return m_db; }
	int RequestNewUserId();
	int GetLocalUserId() { return m_localToolboxId; }
    int GetNumberOfFrames();
	void Reset();
	
	void IncomingNetCommand( drwCommand::s_ptr );
	
public slots:
	
	void IncomingLocalCommand( drwCommand::s_ptr );
	void IncomingDbCommand( drwCommand::s_ptr command );
	
protected:
	
	drwToolbox * AddUser( int commandUserId );
	void ClearAllToolboxesButLocal();
	
	Scene * m_scene;
	drwCommandDatabase * m_db;
	drwNetworkManager * m_netManager;

	// Container to cache state commands until an effective command comes
	typedef QList< drwCommand::s_ptr > CommandContainer;
	CommandContainer m_cachedStateCommands;

	static const int m_localToolboxId;
	int m_lastUsedUserId;
	typedef QMap< int, drwToolbox* > ToolboxContainer;
	ToolboxContainer m_toolboxes;
	
};

#endif
