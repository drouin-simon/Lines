#include "drwToolbox.h"
#include "drwEditionState.h"
#include "drwLineTool.h"

drwToolbox::drwToolbox( int userId, Scene * scene, drwEditionState * editionState, QObject * parent ) 
: drwWidgetObserver( scene, parent )
, m_userId( userId )
, m_editionState( editionState )
{
	if( !m_editionState )
		m_editionState = new drwEditionState(this);
	drwLineTool * drawTool = new drwLineTool( userId, CurrentScene, m_editionState, this );
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

// Implementation of drwWidgetObserver
void drwToolbox::MousePressEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->MousePressEvent( w, e );
}

void drwToolbox::MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->MouseReleaseEvent( w, e );
}

void drwToolbox::MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->MouseMoveEvent( w, e );
}

void drwToolbox::TabletEvent( drwDrawingWidget * w, QTabletEvent * e )
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->TabletEvent( w, e );
}

void drwToolbox::DropEvent( drwDrawingWidget * w, QDropEvent * e )
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->DropEvent( w, e );
}

void drwToolbox::EnterEvent( drwDrawingWidget * w, QEvent * e ) 
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->EnterEvent( w, e );
}

void drwToolbox::LeaveEvent( drwDrawingWidget * w, QEvent * e ) 
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->LeaveEvent( w, e );
}

void drwToolbox::SetCurrentFrame( int frame )
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->SetCurrentFrame( frame );
	
	if( m_editionState )
		m_editionState->SetCurrentFrame( frame );
	
	drwSetFrameCommand * com = new drwSetFrameCommand(m_userId);
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

void drwToolbox::SetPersistence( int persist )
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->SetPersistence( persist );
	
	if( m_editionState )
		m_editionState->SetPersistence( persist );
	
	drwSetPersistenceCommand * com = new drwSetPersistenceCommand(m_userId);
	com->SetPersistence(persist);
	drwCommand::s_ptr command( com );
	emit CommandExecuted( command );
}

void drwToolbox::blockSignals( bool block )
{
	QObject::blockSignals( block );
	for( unsigned i = 0; i < Tools.size(); ++i )
		Tools[i]->blockSignals( block );
}

void drwToolbox::ToggleBrushEraser()
{
	if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->ToggleBrushEraser();
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
	else if( command->GetCommandId() == drwIdSetPersistenceCommand )
	{
		drwSetPersistenceCommand * setPersistCom = dynamic_cast<drwSetPersistenceCommand*> (command.get());
		SetPersistence( setPersistCom->GetPersistence() );
	}
	else if( CurrentTool >= 0 && CurrentTool < Tools.size() )
		Tools[CurrentTool]->ExecuteCommand( command );
}

