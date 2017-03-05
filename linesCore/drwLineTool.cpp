#include "drwLineTool.h"
#include "line.h"
#include "wideline.h"
#include "Scene.h"
#include "Node.h"
#include "drwEditionState.h"
#include <QTabletEvent>
#include <QSettings>
#include <algorithm>

drwLineTool::drwLineTool( Scene * scene, drwEditionState * editionState, QObject * parent )
: drwWidgetObserver( scene, parent )
, m_lastXWorld( 0.0 )
, m_lastYWorld( 0.0 )
, m_lastPressure( 1.0 )
, IsDrawing( false )
, m_tabletHasControl( false )
, Color(1.0,1.0,1.0,1.0)
, Type( TypeWideLine )
, m_baseWidth( 5.0 )
, m_pressureWidth(true)
, m_pressureOpacity(true)
, m_fill(false)
, m_erase(false)
, m_persistence( 0 )
, m_minDistanceBetweenPoints( 4.0 )
, m_minWidth( 2.0 )
, m_maxWidth( 100.0 )
, m_editionState(editionState)
{	
	// Make sure the Reset function is the only one driving initial param values
	Reset();

    connect( m_editionState, SIGNAL(ModifiedSignal()), this, SLOT(OnEditionStateParamsModified()) );
}

void drwLineTool::ReadSettings( QSettings & s )
{
    Color[0] = s.value( "Color.R", Color[0] ).toDouble();
    Color[1] = s.value( "Color.G", Color[1] ).toDouble();
    Color[2] = s.value( "Color.B", Color[2] ).toDouble();
    Color[3] = s.value( "Color.A", Color[3] ).toDouble();
    
    m_baseWidth = s.value( "BaseWidth", QVariant(m_baseWidth) ).toDouble();
    m_pressureWidth = s.value( "PressureWidth", QVariant(m_pressureWidth) ).toBool();
    m_pressureOpacity = s.value( "PressureOpacity", QVariant(m_pressureOpacity) ).toBool();
    m_fill = s.value( "Fill", QVariant(m_fill) ).toBool();
    m_erase = s.value( "Erase", QVariant(m_erase) ).toBool();
    
    ParametersChanged();
}

void drwLineTool::WriteSettings( QSettings & s )
{
    s.setValue( "Color.R", QVariant(Color[0]) );
    s.setValue( "Color.G", QVariant(Color[1]) );
    s.setValue( "Color.B", QVariant(Color[2]) );
    s.setValue( "Color.A", QVariant(Color[3]) );
    
    s.setValue( "BaseWidth", QVariant(m_baseWidth) );
    s.setValue( "PressureWidth", QVariant(m_pressureWidth) );
    s.setValue( "PressureOpacity", QVariant(m_pressureOpacity) );
    s.setValue( "Fill", QVariant(m_fill) );
    s.setValue( "Erase", QVariant(m_erase) );
}

void drwLineTool::StartLine( double xWorld, double yWorld, double pressure )
{
    drwCommand::s_ptr command( new drwMouseCommand( drwMouseCommand::Press, xWorld, yWorld, (int)xWorld, (int)yWorld, 0.0, 0, 0, pressure, 0.0, 0.0 ) );
    ExecuteCommand( command );
}

void drwLineTool::AddPoint( double xWorld, double yWorld, double pressure )
{
    drwCommand::s_ptr command( new drwMouseCommand( drwMouseCommand::Move, xWorld, yWorld, (int)xWorld, (int)yWorld, 0.0, 0, 0, pressure, 0.0, 0.0 ) );
    ExecuteCommand( command );
}

void drwLineTool::EndLine( double xWorld, double yWorld, double pressure )
{
    drwCommand::s_ptr command( new drwMouseCommand( drwMouseCommand::Release, xWorld, yWorld, (int)xWorld, (int)yWorld, 0.0, 0, 0, pressure, 0.0, 0.0 ) );
    ExecuteCommand( command );
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
    m_erase = paramCom->GetErase();
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

        // Mark rect modified in scene
        MarkPointModified( m_lastXWorld, m_lastYWorld );

		emit CommandExecuted( command );
		emit StartInteraction();
	}
    else if( mouseCom->GetType() == drwMouseCommand::Release && IsDrawing )
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
        MarkSegmentModified( m_lastXWorld, m_lastYWorld, mouseCom->X(), mouseCom->Y() );
		CurrentNodes.clear();
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
            MarkSegmentModified( m_lastXWorld, m_lastYWorld, mouseCom->X(), mouseCom->Y() );
            m_lastXWorld = mouseCom->X();
            m_lastYWorld = mouseCom->Y();
            m_lastPressure = mouseCom->Pressure();
            m_lastXPix = mouseCom->XPix();
            m_lastYPix = mouseCom->YPix();
            emit CommandExecuted( command );
		}
	}
}

void drwLineTool::MarkPointModified( double x, double y )
{
    double xMin = x - m_baseWidth;
    double xMax = x + m_baseWidth;
    double yMin = y - m_baseWidth;
    double yMax = y + m_baseWidth;
    Box2d modifiedRect( xMin, xMax, yMin, yMax );
    CurrentNodesCont::iterator it = CurrentNodes.begin();
    while( it != CurrentNodes.end() )
    {
        int frameIndex = it->first;
        CurrentScene->MarkModified( frameIndex, modifiedRect );
        ++it;
    }
}

void drwLineTool::MarkSegmentModified( double x1, double y1, double x2, double y2 )
{
    double xMin = std::min( x1, x2 );
    double xMax = std::max( x1, x2 );
    double yMin = std::min( y1, y2 );
    double yMax = std::max( y1, y2 );
    double xBoxMin = xMin - m_baseWidth;
    double xBoxMax = xMax + m_baseWidth;
    double yBoxMin = yMin - m_baseWidth;
    double yBoxMax = yMax + m_baseWidth;
    Box2d modifiedRect( xBoxMin, xBoxMax, yBoxMin, yBoxMax );
    CurrentNodesCont::iterator it = CurrentNodes.begin();
    while( it != CurrentNodes.end() )
    {
        int frameIndex = it->first;
        CurrentScene->MarkModified( frameIndex, modifiedRect );
        ++it;
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

void drwLineTool::SetErase( bool e )
{
    m_erase = e;
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

void drwLineTool::SetBaseWidth( double newBaseWidth )
{
    m_baseWidth = newBaseWidth;
    if( m_baseWidth < m_minWidth )
        m_baseWidth = m_minWidth;
    if( m_baseWidth > m_maxWidth )
        m_baseWidth = m_maxWidth;
    ParametersChanged();
}

void drwLineTool::OnEditionStateParamsModified()
{
    if( m_editionState->IsPersistenceEnabled() )
        SetPersistence( m_editionState->GetPersistence() );
    else
        SetPersistence( 0 );
}

void drwLineTool::ParametersChanged()
{
	drwLineToolParamsCommand * c = new drwLineToolParamsCommand();
	c->SetColor( Color );
	c->SetBaseWidth( m_baseWidth );
	c->SetPressureWidth( m_pressureWidth );
	c->SetPressureOpacity( m_pressureOpacity );
	c->SetFill( m_fill );
    c->SetErase( m_erase );
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
            newWideLine->SetErase( m_erase );
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
    int lastFrame = m_editionState->GetCurrentFrame() + m_persistence;
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
