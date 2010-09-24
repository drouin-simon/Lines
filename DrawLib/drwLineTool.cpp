#include "drwLineTool.h"
#include "line.h"
#include "wideline.h"
#include "Scene.h"
#include "Node.h"
#include "drwEditionState.h"
#include "drwDrawingWidget.h"
#include <QTabletEvent>

drwLineTool::drwLineTool( int userId, Scene * scene, drwEditionState * editionState, QObject * parent ) 
: drwWidgetObserver( scene, parent )
, Color(1.0,1.0,1.0)
, m_editionState(editionState)
, m_userId( userId )
, m_baseWidth( 10.0 )
{
	IsDrawing = false;
	//Type = TypeLine;
	Type = TypeWideLine;
	LastXWorld = 0;
	LastYWorld = 0;
	LastPressure = 1.0;
	
	// simtodo : fix this
	CurrentScene->SetCursorRadius( m_baseWidth );
}

void drwLineTool::MousePressEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Press, e );
	SetCursorPosition( command );
	ExecuteCommand( command );	
}

void drwLineTool::MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Release, e );
	SetCursorPosition( command );
	ExecuteCommand( command );
}

void drwLineTool::MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Move, e );
	SetCursorPosition( command );
	ExecuteCommand( command );
}

void drwLineTool::TabletEvent( drwDrawingWidget * w, QTabletEvent * e )
{
	if( e->type() == QEvent::TabletPress )
	{
		drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Press, e );
		SetCursorPosition( command );
		ExecuteCommand( command );
		e->accept();
	}
	else if( e->type() == QEvent::TabletRelease )
	{
		drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Release, e );
		SetCursorPosition( command );
		ExecuteCommand( command );
		e->accept();
	}
	else if( e->type() == QEvent::TabletMove )
	{
		drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Move, e );
		SetCursorPosition( command );
		ExecuteCommand( command );
		e->accept();
	}
}
									
void drwLineTool::ExecuteCommand( drwCommand::s_ptr command )
{
	drwMouseCommand * mouseCom = dynamic_cast<drwMouseCommand*>(command.get());
	if( mouseCom )
	{
		if( mouseCom->GetType() == drwMouseCommand::Press )
		{
			IsDrawing = true;
			LastXWorld = mouseCom->X();
			LastYWorld = mouseCom->Y();
			LastPressure = mouseCom->Pressure();
			CreateNewNodes();
			emit CommandExecuted( command );
			emit StartInteraction();
		}
		else if( mouseCom->GetType() == drwMouseCommand::Release )
		{
			CurrentNodesCont::iterator it = CurrentNodes.begin();
			while( it != CurrentNodes.end() )
			{
				LinePrimitive * prim = dynamic_cast<LinePrimitive*> (it->second->GetPrimitive());
				prim->EndPoint( mouseCom->X(), mouseCom->Y(), mouseCom->Pressure() );
				++it;
			}
			CurrentNodes.clear();
			CurrentScene->MarkModified();
			IsDrawing = false;
			
			LastXWorld = mouseCom->X();
			LastYWorld = mouseCom->Y();
			LastPressure = mouseCom->Pressure();
			emit CommandExecuted( command );
			emit EndInteraction();
		}
		else if( mouseCom->GetType() == drwMouseCommand::Move )
		{
			if( IsDrawing )
			{
				CurrentNodesCont::iterator it = CurrentNodes.begin();
				while( it != CurrentNodes.end() )
				{
					LinePrimitive * prim = dynamic_cast<LinePrimitive*> (it->second->GetPrimitive());
					prim->AddPoint( mouseCom->X(), mouseCom->Y(), mouseCom->Pressure() );
					++it;
				}
				CurrentScene->MarkModified();
				LastXWorld = mouseCom->X();
				LastYWorld = mouseCom->Y();
				LastPressure = mouseCom->Pressure();
				emit CommandExecuted( command );
			}
		}
	}
}
			
void drwLineTool::SetCurrentFrame( int frame ) 
{
	// This is for the case where we are drawing while animating
	if( IsDrawing )
	{
		// Terminate line on previous frames that are not active anymore
		int newInterval[2];
		newInterval[0] = m_editionState->GetCurrentFrame();
		newInterval[1] = m_editionState->GetCurrentFrame() + m_editionState->GetPersistence();
		CurrentNodesCont::iterator it = CurrentNodes.begin();
		while( it != CurrentNodes.end() )
		{
			int frame = it->first;
			if( frame >= newInterval[0] && frame <= newInterval[1] )
			{
				++it;
			}
			else
			{
				LinePrimitive * prim = dynamic_cast<LinePrimitive*> (it->second->GetPrimitive());
				prim->EndPoint( LastXWorld, LastYWorld, LastPressure );
				CurrentNodesCont::iterator temp = it;
				++it;
				CurrentNodes.erase( temp );
			}
		}
		
		// Create a new line for new frame
		CreateNewNodes();
	}
}

void drwLineTool::Reset()
{
	Color = Vec3(1.0,1.0,1.0);
	IsDrawing = false;
	Type = TypeLine;
	LastXWorld = 0;
	LastYWorld = 0;
}

Node * drwLineTool::CreateNewNode()
{
	LinePrimitive * newPrimitive = 0;
	switch( Type )
	{
		case TypeLine:
		{
			Line * newLine = new Line;
			newLine->SetWidth( 2 );
			newPrimitive = newLine;
		}
			break;
		case TypeWideLine:
			newPrimitive = new WideLine( m_baseWidth );
			break;
		case EndType:
			break;
	}
	newPrimitive->SetColor( Color );
	newPrimitive->StartPoint( LastXWorld, LastYWorld, LastPressure );
	Node * CurrentNode = new Node;
	CurrentNode->SetPrimitive( newPrimitive );
	return CurrentNode;
}


void drwLineTool::CreateNewNodes( )
{
	int lastFrame = m_editionState->GetCurrentFrame() + m_editionState->GetPersistence();
	for( int frame = m_editionState->GetCurrentFrame(); frame <= lastFrame; ++frame )
	{
		CurrentNodesCont::iterator it = CurrentNodes.find( frame );
		if( it == CurrentNodes.end() )
		{
			Node * newNode = CreateNewNode();
			CurrentScene->AddNodeToFrame( newNode, frame );
			CurrentNodes[frame] = newNode;
		}	
	}
}


void drwLineTool::SetCursorPosition( drwCommand::s_ptr command )
{
	drwMouseCommand * mouseCom = dynamic_cast<drwMouseCommand*>(command.get());
	if( mouseCom )
		CurrentScene->SetCursorPos( mouseCom->X(), mouseCom->Y() );
}

