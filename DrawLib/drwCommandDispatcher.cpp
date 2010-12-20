#include "drwCommandDispatcher.h"
#include "drwToolbox.h"
#include "drwCommandDatabase.h"
#include "drwNetworkManager.h"

const int drwCommandDispatcher::m_localToolboxId = 0;

drwCommandDispatcher::drwCommandDispatcher( drwNetworkManager * net,
											drwCommandDatabase * db,
											drwToolbox * local,
											Scene * scene,
											QObject * parent )
: QObject(parent)
, m_scene( scene )
, m_db( db )
, m_netManager( net )
, m_lastUsedUserId( 0 ) // last used is local
{
	connect( local, SIGNAL(CommandExecuted(drwCommand::s_ptr)), this, SLOT(IncomingLocalCommand(drwCommand::s_ptr)));
	m_toolboxes[ m_localToolboxId ] = local;
}

drwCommandDispatcher::~drwCommandDispatcher()
{
	// Delete all toolboxes but the local toolbox
	drwToolbox * local = m_toolboxes[ m_localToolboxId ];
	foreach( drwToolbox * current, m_toolboxes )
	{
		if( current != local )
			delete current;
	}
}

int drwCommandDispatcher::RequestNewUserId()
{
	return ++m_lastUsedUserId;
}

void drwCommandDispatcher::IncomingNetCommand( drwCommand::s_ptr command )
{
	// Execute the command in the appropriate toolbox
	int commandUserId = command->GetUserId();
	drwToolbox * box = m_toolboxes[ commandUserId ];
	if( !box )
		box = AddUser( commandUserId );
	box->ExecuteCommand( command );

	// Store it in the database
	m_db->PushCommand( command );
}

void drwCommandDispatcher::IncomingLocalCommand( drwCommand::s_ptr command )
{
	// Send it to the network
	m_netManager->SendCommand( command );

	// Store it in the database
	m_db->PushCommand( command );
}

void drwCommandDispatcher::IncomingDbCommand( drwCommand::s_ptr command )
{
	// Execute the command in the appropriate toolbox
	int commandUserId = command->GetUserId();
	drwToolbox * box = m_toolboxes[ commandUserId ];
	if( !box )
		box = AddUser( commandUserId );
	box->ExecuteCommand( command );
}

drwToolbox * drwCommandDispatcher::AddUser( int commandUserId )
{
	drwToolbox * newUser = new drwToolbox( m_scene, NULL, this );
	m_toolboxes[ commandUserId ] = newUser;
	if( m_lastUsedUserId < commandUserId )
		m_lastUsedUserId = commandUserId;
	return newUser;
}
