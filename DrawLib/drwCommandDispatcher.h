#ifndef __drwCommandDispatcher_h_
#define __drwCommandDispatcher_h_

#include <QObject>
#include <vector>
#include "drwCommand.h"

class Scene;
class drwToolbox;

class drwCommandDispatcher : public QObject
{
	
	Q_OBJECT
	
public:
	
	drwCommandDispatcher( drwToolbox * local, Scene * scene, QObject * parent );
	~drwCommandDispatcher();
	
public slots:
	
	void ExecuteCommand( drwCommand::s_ptr );
	
protected:
	
	int GetUser( int userId );
	int AddUser( int commandUserId );
	
	Scene * m_scene;
	std::vector< drwToolbox* > m_users;
	
};

#endif