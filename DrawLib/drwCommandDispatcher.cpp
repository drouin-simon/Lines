#include "drwCommandDispatcher.h"
#include "drwToolbox.h"

drwCommandDispatcher::drwCommandDispatcher( drwToolbox * local, Scene * scene, QObject * parent )
: QObject(parent)
, m_scene( scene )
{
	m_users.push_back( local );
}

drwCommandDispatcher::~drwCommandDispatcher()
{
}

void drwCommandDispatcher::ExecuteCommand( drwCommand::s_ptr command )
{
	int commandUserId = command->GetUserId();
	int userIndex = GetUser( commandUserId );
	if( userIndex == -1 )
		userIndex = AddUser( commandUserId );
	m_users[userIndex]->ExecuteCommand( command );
}

int drwCommandDispatcher::GetUser( int userId )
{
	for( unsigned i = 0; i < m_users.size(); ++i )
	{
		if( m_users[i]->GetUserId() == userId )
			return i;
	}
	return -1;
}

int drwCommandDispatcher::AddUser( int commandUserId )
{
	drwToolbox * newUser = new drwToolbox( commandUserId, m_scene, NULL, this );
	m_users.push_back( newUser );
	return m_users.size() - 1;
}
