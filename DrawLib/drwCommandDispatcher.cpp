#include "drwCommandDispatcher.h"
#include "drwToolbox.h"
#include "drwCommandDatabase.h"
#include "drwNetworkManager.h"

drwCommandDispatcher::drwCommandDispatcher( drwNetworkManager * net,
											drwCommandDatabase * db,
											drwToolbox * local,
											Scene * scene,
											QObject * parent )
: QObject(parent)
, m_scene( scene )
, m_db( db )
, m_netManager( net )
, m_localToolboxId(0)
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
	drwToolbox * newUser = new drwToolbox( commandUserId, m_scene, NULL, this );
	m_toolboxes[ commandUserId ] = newUser;
	return newUser;
}
