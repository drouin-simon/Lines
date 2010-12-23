#include "drwLineTool.h"
#include "line.h"
#include "wideline.h"
#include "Scene.h"
#include "Node.h"
#include "drwEditionState.h"
#include "drwDrawingWidget.h"
#include "drwCursor.h"
#include <QTabletEvent>

drwLineTool::drwLineTool( Scene * scene, drwEditionState * editionState, QObject * parent )
: drwWidgetObserver( scene, parent )
, m_cursorShouldAppear( false )
, Color(1.0,1.0,1.0)
, LastXWorld( 0.0 )
, LastYWorld( 0.0 )
, LastPressure( 1.0 )
, IsDrawing( false )
, Type( TypeWideLine )
, m_isEraser( false )
, m_baseWidth( 10.0 )
, m_minWidth( 2.0 )
, m_maxWidth( 100.0 )
, m_brushScaling( false )
, m_lastXWin( 0 )
, m_lastYWin( 0 )
, m_editionState(editionState)
{	
	// simtodo : fix this
	CurrentScene->GetCursor()->SetRadius( m_baseWidth );
}

void drwLineTool::MousePressEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	if( e->modifiers() & Qt::AltModifier )
		BrushWidthStart( e->x(), e->y() );
	else 
	{
		drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Press, e );
		SetCursorPosition( command );
		ExecuteCommand( command );
	}
}

void drwLineTool::MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	if( m_brushScaling )
		BrushWidthEnd( e->x(), e->y() );
	else 
	{
		drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Release, e );
		SetCursorPosition( command );
		ExecuteCommand( command );
	}
}

void drwLineTool::MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	if( m_brushScaling )
		BrushWidthMove( e->x(), e->y() );
	else 
	{
		drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Move, e );
		SetCursorPosition( command );
		ExecuteCommand( command );
	}
}

void drwLineTool::TabletEvent( drwDrawingWidget * w, QTabletEvent * e )
{
	if( e->modifiers() & Qt::AltModifier && e->type() == QEvent::TabletPress )
		BrushWidthStart( e->x(), e->y() );
	else if( m_brushScaling )
	{
		if( e->type() == QEvent::TabletRelease )
			BrushWidthEnd( e->x(), e->y() );
		else if( e->type() == QEvent::TabletMove )
			BrushWidthMove( e->x(), e->y() );
	}
	else 
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
}

void drwLineTool::EnterEvent( drwDrawingWidget * w, QEvent * e ) 
{
	m_cursorShouldAppear = true;
}


void drwLineTool::LeaveEvent( drwDrawingWidget * w, QEvent * e ) 
{
	if( m_cursorShouldAppear )
		m_cursorShouldAppear = false;
	if( !m_brushScaling )
		CurrentScene->SetCursorVisible( false );
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
	Type = TypeWideLine;
	LastXWorld = 0;
	LastYWorld = 0;
	m_baseWidth = 10.0;
}


void drwLineTool::ToggleBrushEraser()
{
	if( m_isEraser )
		m_isEraser = false;
	else
		m_isEraser = true;
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
			newPrimitive = new WideLine( m_baseWidth, m_isEraser );
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
	{
		if( m_cursorShouldAppear )
		{
			CurrentScene->SetCursorVisible( true );
			m_cursorShouldAppear = false;
		}
		CurrentScene->GetCursor()->SetPosition( mouseCom->X(), mouseCom->Y() );
		CurrentScene->MarkModified();
	}
}

void drwLineTool::BrushWidthStart( int x, int y )
{
	m_lastXWin = x;
	m_lastYWin = y;
	m_brushScaling = true;
	CurrentScene->GetCursor()->SetShowArrow( true );
	CurrentScene->MarkModified();
}

void drwLineTool::BrushWidthEnd( int x, int y )
{
	m_brushScaling = false;
	CurrentScene->GetCursor()->SetShowArrow( false );
	CurrentScene->MarkModified();
}

void drwLineTool::BrushWidthMove( int x, int y )
{
	if( m_brushScaling )
	{
		double diffY = y - m_lastYWin;
		m_baseWidth += diffY * -0.2;
		if( m_baseWidth < m_minWidth )
			m_baseWidth = m_minWidth;
		if( m_baseWidth > m_maxWidth )
			m_baseWidth = m_maxWidth;
		CurrentScene->GetCursor()->SetRadius( m_baseWidth );
		CurrentScene->MarkModified();
		m_lastXWin = x;
		m_lastYWin = y;
	}
}

