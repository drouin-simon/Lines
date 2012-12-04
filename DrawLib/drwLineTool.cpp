#include "drwLineTool.h"
#include "line.h"
#include "wideline.h"
#include "Scene.h"
#include "Node.h"
#include "drwEditionState.h"
#include "drwDrawingWidget.h"
#include <QTabletEvent>

drwLineTool::drwLineTool( Scene * scene, drwEditionState * editionState, QObject * parent )
: drwWidgetObserver( scene, parent )
, m_lastXWorld( 0.0 )
, m_lastYWorld( 0.0 )
, m_lastPressure( 1.0 )
, IsDrawing( false )
, Color(1.0,1.0,1.0,1.0)
, Type( TypeWideLine )
, m_baseWidth( 10.0 )
, m_pressureWidth(true)
, m_pressureOpacity(true)
, m_fill(false)
, m_persistence( 0 )
, m_minDistanceBetweenPoints( 4.0 )
, m_minWidth( 2.0 )
, m_maxWidth( 100.0 )
, m_editionState(editionState)
{	
	// Make sure the Reset function is the only one driving initial param values
	Reset();
}

void drwLineTool::StartLine( double xWorld, double yWorld )
{
    drwCommand::s_ptr command( new drwMouseCommand( drwMouseCommand::Press, xWorld, yWorld, (int)xWorld, (int)yWorld, 0.0, 0, 0, 1.0, 0.0, 0.0 ) );
    ExecuteCommand( command );
}

void drwLineTool::AddPoint( double xWorld, double yWorld )
{
    drwCommand::s_ptr command( new drwMouseCommand( drwMouseCommand::Move, xWorld, yWorld, (int)xWorld, (int)yWorld, 0.0, 0, 0, 1.0, 0.0, 0.0 ) );
    ExecuteCommand( command );
}

void drwLineTool::EndLine( double xWorld, double yWorld )
{
    drwCommand::s_ptr command( new drwMouseCommand( drwMouseCommand::Release, xWorld, yWorld, (int)xWorld, (int)yWorld, 0.0, 0, 0, 1.0, 0.0, 0.0 ) );
    ExecuteCommand( command );
}

void drwLineTool::MousePressEvent( drwDrawingWidget * w, QMouseEvent * e )
{
    drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Press, e );
    ExecuteCommand( command );
}

void drwLineTool::MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e )
{
    drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Release, e );
    ExecuteCommand( command );
}

void drwLineTool::MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e )
{
    drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Move, e );
    ExecuteCommand( command );
}

void drwLineTool::TabletEvent( drwDrawingWidget * w, QTabletEvent * e )
{
    if( e->type() == QEvent::TabletPress )
    {
        drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Press, e );
        ExecuteCommand( command );
        e->accept();
    }
    else if( e->type() == QEvent::TabletRelease )
    {
        drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Release, e );
        ExecuteCommand( command );
        e->accept();
    }
    else if( e->type() == QEvent::TabletMove )
    {
        drwCommand::s_ptr command = w->CreateMouseCommand( drwMouseCommand::Move, e );
        ExecuteCommand( command );
        e->accept();
    }
}

void drwLineTool::ExecuteCommand( drwCommand::s_ptr command )
{
	if( command->GetCommandId() == drwIdMouseCommand )
		ExecuteMouseCommand( command );
	else if( command->GetCommandId() == drwIdLineToolParamsCommand )
		ExecuteLineToolParamCommand( command );
}

void drwLineTool::ExecuteLineToolParamCommand( drwCommand::s_ptr command )
{
	drwLineToolParamsCommand * paramCom = dynamic_cast<drwLineToolParamsCommand*>(command.get());
	Q_ASSERT( paramCom );
	Color = paramCom->GetColor();
	m_baseWidth = paramCom->GetBaseWidth();
	m_pressureWidth = paramCom->GetPressureWidth();
	m_pressureOpacity = paramCom->GetPressureOpacity();
	m_fill = paramCom->GetFill();
	m_persistence = paramCom->GetPersistence();
}

void drwLineTool::ExecuteMouseCommand( drwCommand::s_ptr command )
{
	drwMouseCommand * mouseCom = dynamic_cast<drwMouseCommand*>(command.get());
	Q_ASSERT( mouseCom );


	if( mouseCom->GetType() == drwMouseCommand::Press )
	{
		IsDrawing = true;
        m_lastXWorld = mouseCom->X();
        m_lastYWorld = mouseCom->Y();
        m_lastPressure = mouseCom->Pressure();
        m_lastXPix = mouseCom->XPix();
        m_lastYPix = mouseCom->YPix();
		CreateNewNodes();
        CurrentScene->MarkModified();
		emit CommandExecuted( command );
		emit StartInteraction();
	}
	else if( mouseCom->GetType() == drwMouseCommand::Release )
	{
		CurrentNodesCont::iterator it = CurrentNodes.begin();
		while( it != CurrentNodes.end() )
		{
            int nodeId = it->second;
            int frameIndex = it->first;

            Node * n = CurrentScene->LockNode( frameIndex, nodeId );
            LinePrimitive * prim = dynamic_cast<LinePrimitive*> (n->GetPrimitive());
            Q_ASSERT( prim );
            prim->EndPoint( mouseCom->X(), mouseCom->Y(), mouseCom->Pressure() );
            CurrentScene->UnlockNode( frameIndex, nodeId );

			++it;
		}
		CurrentNodes.clear();
        CurrentScene->MarkModified();
		IsDrawing = false;

        m_lastXWorld = mouseCom->X();
        m_lastYWorld = mouseCom->Y();
        m_lastPressure = mouseCom->Pressure();
		emit CommandExecuted( command );
		emit EndInteraction();
	}
	else if( mouseCom->GetType() == drwMouseCommand::Move )
	{
		if( IsDrawing )
		{
            double diffX = (double)( mouseCom->XPix() - m_lastXPix );
            double diffY = (double)( mouseCom->YPix() - m_lastYPix );
            double distWithLast = sqrt( diffX*diffX + diffY*diffY );
            if( distWithLast  > m_minDistanceBetweenPoints )
            {

                CurrentNodesCont::iterator it = CurrentNodes.begin();
                while( it != CurrentNodes.end() )
                {
                    int nodeId = it->second;
                    int frameIndex = it->first;

                    Node * n = CurrentScene->LockNode( frameIndex, nodeId );
                    LinePrimitive * prim = dynamic_cast<LinePrimitive*> (n->GetPrimitive());
                    Q_ASSERT( prim );
                    prim->AddPoint( mouseCom->X(), mouseCom->Y(), mouseCom->Pressure() );
                    CurrentScene->UnlockNode( frameIndex, nodeId );

                    ++it;
                }
                CurrentScene->MarkModified();
                m_lastXWorld = mouseCom->X();
                m_lastYWorld = mouseCom->Y();
                m_lastPressure = mouseCom->Pressure();
                m_lastXPix = mouseCom->XPix();
                m_lastYPix = mouseCom->YPix();
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
		newInterval[1] = m_editionState->GetCurrentFrame() + m_persistence;
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
                int nodeId = it->second;

                // Lock node and make changes
                Node * n = CurrentScene->LockNode( frame, nodeId );
                LinePrimitive * prim = dynamic_cast<LinePrimitive*> (n->GetPrimitive());
                Q_ASSERT( prim );
                prim->EndPoint( m_lastXWorld, m_lastYWorld, m_lastPressure );
                CurrentScene->UnlockNode( frame, nodeId );

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
    m_lastXWorld = 0.0;
    m_lastYWorld = 0.0;
    m_lastPressure = 1.0;
	IsDrawing = false;
	Color = Vec4(1.0,1.0,1.0,1.0);
	Type = TypeWideLine;
	m_baseWidth = 10.0;
	m_pressureWidth = true;
	m_pressureOpacity = true;
	m_fill = false;
	m_persistence = 0;
    CurrentNodes.clear();
}

void drwLineTool::SetPressureWidth( bool w )
{
	m_pressureWidth = w;
	ParametersChanged();
}

void drwLineTool::SetPressureOpacity( bool o )
{
	m_pressureOpacity = o;
	ParametersChanged();
}

bool drwLineTool::IsPerssureWidthAndOpacityEnabled()
{
    return !GetFill();
}

void drwLineTool::SetFill( bool f )
{
	m_fill = f;
	ParametersChanged();
}

void drwLineTool::SetColor( Vec4 & c )
{
	Color = c;
	ParametersChanged();
}

void drwLineTool::SetPersistence( int p )
{
	m_persistence = p;
	ParametersChanged();
}

bool drwLineTool::IsPersistenceEnabled()
{
    return ( m_editionState->GetFrameChangeMode() == Play );
}

void drwLineTool::SetBaseWidth( double newBaseWidth )
{
    m_baseWidth = newBaseWidth;
    if( m_baseWidth < m_minWidth )
        m_baseWidth = m_minWidth;
    if( m_baseWidth > m_maxWidth )
        m_baseWidth = m_maxWidth;
    ParametersChanged();
}

void drwLineTool::ParametersChanged()
{
	drwLineToolParamsCommand * c = new drwLineToolParamsCommand();
	c->SetColor( Color );
	c->SetBaseWidth( m_baseWidth );
	c->SetPressureWidth( m_pressureWidth );
	c->SetPressureOpacity( m_pressureOpacity );
	c->SetFill( m_fill );
	c->SetPersistence( m_persistence );
	drwCommand::s_ptr command( c );
	emit CommandExecuted( command );
	emit ParametersChangedSignal();
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
		{
			WideLine * newWideLine = new WideLine( m_baseWidth );
            if( IsPerssureWidthAndOpacityEnabled() )
            {
                newWideLine->SetPressureWidth( m_pressureWidth );
                newWideLine->SetPressureOpacity( m_pressureOpacity );
            }
            else
            {
                newWideLine->SetPressureWidth( false );
                newWideLine->SetPressureOpacity( false );
            }
			newWideLine->SetFill( m_fill );
			newPrimitive = newWideLine;
		}
			break;
		case EndType:
			break;
	}
	newPrimitive->SetColor( Color );
    newPrimitive->StartPoint( m_lastXWorld, m_lastYWorld, m_lastPressure );
	Node * CurrentNode = new Node;
	CurrentNode->SetPrimitive( newPrimitive );
	return CurrentNode;
}


void drwLineTool::CreateNewNodes( )
{
    int lastFrame = m_editionState->GetCurrentFrame();
    if( this->IsPersistenceEnabled() )
        lastFrame += m_persistence;
    if( lastFrame >= CurrentScene->GetNumberOfFrames() )
        lastFrame = CurrentScene->GetNumberOfFrames() - 1;
	for( int frame = m_editionState->GetCurrentFrame(); frame <= lastFrame; ++frame )
	{
		CurrentNodesCont::iterator it = CurrentNodes.find( frame );
		if( it == CurrentNodes.end() )
		{
			Node * newNode = CreateNewNode();
            int nodeId = CurrentScene->AddNodeToFrame( newNode, frame );
            CurrentNodes[frame] = nodeId;
		}	
    }
}


