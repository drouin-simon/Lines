#include "drwCommandDispatcher.h"
#include "drwToolbox.h"
#include "drwCommandDatabase.h"
#include "drwRemoteCommandIO.h"
#include "Scene.h"

const int drwCommandDispatcher::m_localToolboxId = 0;

drwCommandDispatcher::drwCommandDispatcher( drwCommandDatabase * db,
											drwToolbox * local,
											Scene * scene,
											QObject * parent )
: QObject(parent)
, m_scene( scene )
, m_db( db )
, m_remoteIO( 0 )
, m_lastUsedUserId( 0 ) // last used is local
{
	connect( local, SIGNAL(CommandExecuted(drwCommand::s_ptr)), this, SLOT(IncomingLocalCommand(drwCommand::s_ptr)));
    connect( scene, SIGNAL(CommandExecuted(drwCommand::s_ptr)), this, SLOT(IncomingLocalCommand(drwCommand::s_ptr)));
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

int drwCommandDispatcher::GetNumberOfFrames()
{
    return m_scene->GetNumberOfFrames();
}

void drwCommandDispatcher::Reset()
{
    // todo : This should probably be done in network manager
    if( m_remoteIO->IsSharing() )
    {
        drwCommand::s_ptr newSceneCommand( new drwNewSceneCommand );
        m_remoteIO->SendCommand( newSceneCommand );
    }
	m_scene->Clear();
	m_db->Clear();
	ClearAllToolboxesButLocal();
	drwToolbox * local = m_toolboxes[ m_localToolboxId ];
	local->Reset();
	m_lastUsedUserId = 0;
	m_cachedStateCommands.clear();
}

void drwCommandDispatcher::IncomingNetCommand( drwCommand::s_ptr command )
{
    if( command->GetCommandId() == drwIdServerInitialCommand )
    {
        drwServerInitialCommand * serverMsg = dynamic_cast<drwServerInitialCommand*> (command.get());
    }
    else if( command->GetCommandId() == drwIdNewSceneCommand )
    {
        Reset();
    }
    else if( command->GetCommandId() == drwIdSceneParamsCommand )
    {
        drwSceneParamsCommand * serverMsg = dynamic_cast<drwSceneParamsCommand*> (command.get());
        m_scene->SetNumberOfFrames( serverMsg->GetNumberOfFrames() );

        // Store it in the database
        m_db->PushCommand( command );
    }
    else
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
}

void drwCommandDispatcher::IncomingLocalCommand( drwCommand::s_ptr command )
{
	if( command->IsStateCommand() )
	{
		// state command, we cache it for later

		// try to concatenate the command with another one in the stack
		bool found = false;
		for( int i = 0; i < m_cachedStateCommands.size(); ++i )
		{
			if( m_cachedStateCommands[i]->Concatenate(command.get()) )
			{
				found = true;
				break;
			}
		}

		// if no command of the same type is in the stack, push_back
		if( !found )
			m_cachedStateCommands.push_back( command );
	}
	else
	{
		// Send state commands on the stack
		for( int i = 0; i < m_cachedStateCommands.size(); ++i )
		{
            m_remoteIO->SendCommand( m_cachedStateCommands[i] );
			m_db->PushCommand( m_cachedStateCommands[i] );
		}
		m_cachedStateCommands.clear();

		// Send it to the network
        if( m_remoteIO )
            m_remoteIO->SendCommand( command );

		// Store it in the database
		m_db->PushCommand( command );
	}
}

void drwCommandDispatcher::IncomingDbCommand( drwCommand::s_ptr command )
{
    if( command->GetCommandId() == drwIdSceneParamsCommand )
    {
        drwSceneParamsCommand * serverMsg = dynamic_cast<drwSceneParamsCommand*> (command.get());
        m_scene->SetNumberOfFrames( serverMsg->GetNumberOfFrames() );
    }
    else
    {
        // Execute the command in the appropriate toolbox
        int commandUserId = command->GetUserId();
        drwToolbox * box = m_toolboxes[ commandUserId ];
        if( !box )
            box = AddUser( commandUserId );
        box->ExecuteCommand( command );
    }
}

drwToolbox * drwCommandDispatcher::AddUser( int commandUserId )
{
	drwToolbox * newUser = new drwToolbox( m_scene, NULL );
	m_toolboxes[ commandUserId ] = newUser;
	if( m_lastUsedUserId < commandUserId )
		m_lastUsedUserId = commandUserId;
	return newUser;
}

void drwCommandDispatcher::ClearAllToolboxesButLocal()
{
	drwToolbox * local = m_toolboxes[ m_localToolboxId ];
	foreach( drwToolbox * current, m_toolboxes )
	{
		if( current != local )
			delete current;
	}
	m_toolboxes.clear();
	m_toolboxes[ m_localToolboxId ] = local;
}

