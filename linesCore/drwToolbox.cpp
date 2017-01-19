#include "drwToolbox.h"
#include "drwEditionState.h"
#include "drwLineTool.h"

drwToolbox::drwToolbox( Scene * scene, drwEditionState * editionState, QObject * parent )
: drwWidgetObserver( scene, parent )
, m_editionState( editionState )
{
	if( !m_editionState )
		m_editionState = new drwEditionState(this);
	drwLineTool * drawTool = new drwLineTool( CurrentScene, m_editionState, this );
	AddTool( drawTool );
	CurrentTool = 0;
}

drwToolbox::~drwToolbox()
{
	for( unsigned i = 0; i < Tools.size(); ++i )
		delete Tools[i];
}

void drwToolbox::AddTool( drwWidgetObserver * tool )
{
	connect( tool, SIGNAL(CommandExecuted(drwCommand::s_ptr)), SLOT(NotifyCommandExecuted(drwCommand::s_ptr)) );
	connect( tool, SIGNAL(StartInteraction()), SLOT(NotifyStartInteraction()) );
	connect( tool, SIGNAL(EndInteraction()), SLOT(NotifyEndInteraction()) );
	Tools.push_back( tool );
}

drwWidgetObserver * drwToolbox::GetTool( int index )
{
	if( index >= 0 && index < Tools.size() )
		return Tools[ index ];
	return 0;
}

void drwToolbox::ReadSettings( QSettings & s )
{
    for( unsigned i = 0; i < Tools.size(); ++i )
        Tools[i]->ReadSettings( s );

    m_editionState->ReadSettings( s );
}

void drwToolbox::WriteSettings( QSettings & s )
{
    for( unsigned i = 0; i < Tools.size(); ++i )
        Tools[i]->WriteSettings( s );
    m_editionState->WriteSettings( s );
}

void drwToolbox::SetCurrentFrame( int frame )
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->SetCurrentFrame( frame );
	
	if( m_editionState )
		m_editionState->SetCurrentFrame( frame );
	
	drwSetFrameCommand * com = new drwSetFrameCommand();
	com->SetNewFrame(frame);
	drwCommand::s_ptr command( com );
	emit CommandExecuted( command );
}

void drwToolbox::Reset()
{
	CurrentTool = 0;
	for( unsigned i = 0; i < Tools.size(); ++i )
		Tools[i]->Reset();
	m_editionState->Reset();
}

void drwToolbox::blockSignals( bool block )
{
	QObject::blockSignals( block );
	for( unsigned i = 0; i < Tools.size(); ++i )
		Tools[i]->blockSignals( block );
}

void drwToolbox::NotifyCommandExecuted( drwCommand::s_ptr command )
{
	emit CommandExecuted( command );
}

void drwToolbox::NotifyStartInteraction()
{
	emit StartInteraction();
}

void drwToolbox::NotifyEndInteraction()
{
	emit EndInteraction();
}

void drwToolbox::ExecuteCommand( drwCommand::s_ptr command )
{
	if( command->GetCommandId() == drwIdSetFrameCommand )
	{
		drwSetFrameCommand * setFrameCom = dynamic_cast<drwSetFrameCommand*> (command.get());
		SetCurrentFrame( setFrameCom->GetNewFrame() );
	}
	else if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->ExecuteCommand( command );
}

