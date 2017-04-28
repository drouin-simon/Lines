#include "drwLineTool.h"
#include "drwToolbox.h"
#include "drwGLRenderer.h"
#include "line.h"
#include "wideline.h"
#include "Scene.h"
#include "Node.h"
#include "lcCircle.h"
#include <QTabletEvent>
#include <QSettings>
#include <algorithm>

static Vec4 lightGreen = Vec4( 0.5, 1.0, 0.0, 1.0 );
static Vec4 orange = Vec4( 1.0, 0.6, 0.0, 1.0 );

drwLineTool::drwLineTool( Scene * scene, drwToolbox * toolbox )
: drwTool( scene, toolbox )
, m_lastXWorld( 0.0 )
, m_lastYWorld( 0.0 )
, m_lastPressure( 1.0 )
, m_isDrawing( false )
, m_tabletHasControl( false )
, Color(1.0,1.0,1.0,1.0)
, Type( TypeWideLine )
, m_baseWidth( 5.0 )
, m_pressureWidth(true)
, m_pressureOpacity(true)
, m_fill(false)
, m_erase(false)
, m_frameChangeMode( Manual )
, m_persistenceEnabled( false )
, m_persistence( 0 )
, m_minWidth( 2.0 )
, m_maxWidth( 100.0 )
, m_cursor( 0 )
{	
    if( IsLocal() )
    {
        m_cursor = new Circle;
        m_cursor->SetContour( true );
        m_cursor->SetFill( false );
        m_cursor->SetRadius( m_baseWidth );
        m_cursor->SetColor( lightGreen );
    }

	// Make sure the Reset function is the only one driving initial param values
	Reset();
}

void drwLineTool::ReadSettings( QSettings & s )
{
    Color[0] = s.value( "Color.R", Color[0] ).toDouble();
    Color[1] = s.value( "Color.G", Color[1] ).toDouble();
    Color[2] = s.value( "Color.B", Color[2] ).toDouble();
    Color[3] = s.value( "Color.A", Color[3] ).toDouble();
    
    double baseWidth = s.value( "BaseWidth", QVariant(m_baseWidth) ).toDouble();
    SetBaseWidth( baseWidth );
    m_pressureWidth = s.value( "PressureWidth", QVariant(m_pressureWidth) ).toBool();
    m_pressureOpacity = s.value( "PressureOpacity", QVariant(m_pressureOpacity) ).toBool();
    m_fill = s.value( "Fill", QVariant(m_fill) ).toBool();
    m_erase = s.value( "Erase", QVariant(m_erase) ).toBool();
    UpdateCursorColor();
    m_persistence = s.value( "Persistence", QVariant(m_persistence) ).toInt();
    
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
    s.setValue( "Persistence", QVariant(m_persistence) );
}

void drwLineTool::OnStartPlaying()
{
    SetPersistenceEnabled( true );
}

void drwLineTool::OnStopPlaying()
{
    SetPersistenceEnabled( false );
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
    SetBaseWidth( paramCom->GetBaseWidth() );
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

    double xWorld = mouseCom->X();
    double yWorld = mouseCom->Y();
    double pressure = mouseCom->Pressure();

    if( m_cursor )
        m_cursor->SetCenter( xWorld, yWorld );

	if( mouseCom->GetType() == drwMouseCommand::Press )
	{
        // Start drawing
        m_isDrawing = true;

        // Start playback in play mode
        if( m_frameChangeMode == Play && !m_toolbox->IsPlaying() )
        {
            m_interactionStartFrame = m_toolbox->GetCurrentFrame();
            m_toolbox->StartPlaying();
        }

        // Create nodes that contain new line primitives
        CreateNewNodes( xWorld, yWorld, pressure );

        // Mark rect modified in scene
        MarkPointModified( xWorld, yWorld );

        // Propagate command
		emit CommandExecuted( command );
	}
    else if( mouseCom->GetType() == drwMouseCommand::Release && m_isDrawing )
	{
        // Complete all lines started
		CurrentNodesCont::iterator it = CurrentNodes.begin();
		while( it != CurrentNodes.end() )
		{
            int nodeId = it->second;
            int frameIndex = it->first;

            Node * n = m_scene->LockNode( frameIndex, nodeId );
            LinePrimitive * prim = dynamic_cast<LinePrimitive*> (n->GetPrimitive());
            Q_ASSERT( prim );
            prim->EndPoint( xWorld, yWorld, pressure );
            m_scene->UnlockNode( frameIndex, nodeId );

			++it;
		}

        // Mark modified rect
        if( !m_fill )
            MarkSegmentModified( m_lastXWorld, m_lastYWorld, xWorld, yWorld );
        else
            MarkWholePrimitiveModified();

        // Clear the list of nodes where drawing
        CurrentNodes.clear();

        // Stop drawing
        m_isDrawing = false;

        // Propagate command
        emit CommandExecuted( command );

        // Adjust playback and current frame according to the drawing mode
        if( m_frameChangeMode == Play )
        {
            m_toolbox->StopPlaying();
            m_toolbox->SetCurrentFrame( m_interactionStartFrame );
        }
        if( m_frameChangeMode == AfterIntervention && !m_toolbox->IsPlaying() )
            m_toolbox->GotoNextFrame();
	}
	else if( mouseCom->GetType() == drwMouseCommand::Move )
	{
        if( m_isDrawing )
		{
            // Add segments to all ongoing lines
            CurrentNodesCont::iterator it = CurrentNodes.begin();
            while( it != CurrentNodes.end() )
            {
                int nodeId = it->second;
                int frameIndex = it->first;
                
                Node * n = m_scene->LockNode( frameIndex, nodeId );
                LinePrimitive * prim = dynamic_cast<LinePrimitive*> (n->GetPrimitive());
                Q_ASSERT( prim );
                prim->AddPoint( xWorld, yWorld, pressure );
                m_scene->UnlockNode( frameIndex, nodeId );
                
                ++it;
            }

            // Mark modified part of the scene
            MarkSegmentModified( m_lastXWorld, m_lastYWorld, xWorld, yWorld );

            // Propagate command
            emit CommandExecuted( command );
		}
	}

    // In all cases, we want to repaint areas where the cursor is an was
    MarkOverlaySegmentModified( m_lastXWorld, m_lastYWorld, xWorld, yWorld );

    m_lastXWorld = xWorld;
    m_lastYWorld = yWorld;
    m_lastPressure = pressure;
}



void drwLineTool::NotifyFrameChanged( int frame )
{
	// This is for the case where we are drawing while animating
    if( m_isDrawing )
	{
		// Terminate line on previous frames that are not active anymore
		int newInterval[2];
        newInterval[0] = m_toolbox->GetCurrentFrame();
        newInterval[1] = m_toolbox->GetCurrentFrame() + m_persistence;
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
                Node * n = m_scene->LockNode( frame, nodeId );
                LinePrimitive * prim = dynamic_cast<LinePrimitive*> (n->GetPrimitive());
                Q_ASSERT( prim );
                prim->EndPoint( m_lastXWorld, m_lastYWorld, m_lastPressure );
                m_scene->UnlockNode( frame, nodeId );

                CurrentNodesCont::iterator temp = it;
				++it;
				CurrentNodes.erase( temp );
			}
		}
		
		// Create a new line for new frame
        CreateNewNodes( m_lastXWorld, m_lastYWorld, m_lastPressure );
	}
}

void drwLineTool::Reset()
{
    m_lastXWorld = 0.0;
    m_lastYWorld = 0.0;
    m_lastPressure = 1.0;
    m_isDrawing = false;
	Color = Vec4(1.0,1.0,1.0,1.0);
	Type = TypeWideLine;
    SetBaseWidth( 10.0 );
	m_pressureWidth = true;
	m_pressureOpacity = true;
	m_fill = false;
    m_frameChangeMode = Manual;
    m_persistenceEnabled = false;
    m_persistence = 0;
    CurrentNodes.clear();
    ParametersChanged();
}

void drwLineTool::NotifyRendererChanged()
{
    drwGLRenderer * ren = m_toolbox->GetRenderer();
    if( ren && IsLocal() )
        ren->SetCursor( m_cursor );
}

void drwLineTool::SetShowCursor( bool show )
{
    drwGLRenderer * ren = m_toolbox->GetRenderer();
    if( ren )
    {
        m_toolbox->GetRenderer()->SetShowCursor( show );
        double xBoxMin = m_lastXWorld - m_baseWidth;
        double xBoxMax = m_lastXWorld + m_baseWidth;
        double yBoxMin = m_lastYWorld - m_baseWidth;
        double yBoxMax = m_lastYWorld + m_baseWidth;
        Box2d modifiedRect( xBoxMin, xBoxMax, yBoxMin, yBoxMax );
        ren->MarkOverlayModified( modifiedRect );
    }
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
    UpdateCursorColor();
    ParametersChanged();
}

void drwLineTool::SetColor( Vec4 & c )
{
	Color = c;
	ParametersChanged();
}

void drwLineTool::SetFrameChangeMode( drwFrameChangeMode mode )
{
    m_frameChangeMode = mode;
    ParametersChanged();
}

void drwLineTool::SetPersistence( int p )
{
	m_persistence = p;
	ParametersChanged();
}

void drwLineTool::SetPersistenceEnabled( bool enable )
{
    m_persistenceEnabled = enable;
    ParametersChanged();
}

void drwLineTool::SetBaseWidth( double newBaseWidth )
{
    double maxWidth = std::max( m_baseWidth, newBaseWidth );
    m_baseWidth = newBaseWidth;
    if( m_baseWidth < m_minWidth )
        m_baseWidth = m_minWidth;
    if( m_baseWidth > m_maxWidth )
        m_baseWidth = m_maxWidth;
    ParametersChanged();
    if( m_cursor )
        m_cursor->SetRadius( m_baseWidth );

    // Mark overlay modified
    drwGLRenderer * ren = m_toolbox->GetRenderer();
    if( !ren )
        return;
    double xBoxMin = m_lastXWorld - maxWidth;
    double xBoxMax = m_lastXWorld + maxWidth;
    double yBoxMin = m_lastYWorld - maxWidth;
    double yBoxMax = m_lastYWorld + maxWidth;
    Box2d modifiedRect( xBoxMin, xBoxMax, yBoxMin, yBoxMax );
    ren->MarkOverlayModified( modifiedRect );
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
    c->SetPersistence( m_persistenceEnabled ? m_persistence : 0 );
	drwCommand::s_ptr command( c );
	emit CommandExecuted( command );
	emit ParametersChangedSignal();
}

Node * drwLineTool::CreateNewNode( double x, double y, double pressure )
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
            newWideLine->SetPressureWidth( m_pressureWidth && !GetFill() );
            newWideLine->SetPressureOpacity( m_pressureOpacity && !GetFill() && !m_erase );
			newWideLine->SetFill( m_fill );
            newWideLine->SetErase( m_erase );
			newPrimitive = newWideLine;
		}
			break;
		case EndType:
			break;
	}
	newPrimitive->SetColor( Color );
    newPrimitive->StartPoint( x, y, pressure );
	Node * CurrentNode = new Node;
	CurrentNode->SetPrimitive( newPrimitive );
	return CurrentNode;
}


void drwLineTool::CreateNewNodes( double x, double y, double pressure )
{
    int persistence = m_persistenceEnabled ? m_persistence : 0;
    int lastFrame = m_toolbox->GetCurrentFrame() + persistence;
    if( lastFrame >= m_scene->GetNumberOfFrames() )
        lastFrame = m_scene->GetNumberOfFrames() - 1;
    for( int frame = m_toolbox->GetCurrentFrame(); frame <= lastFrame; ++frame )
	{
		CurrentNodesCont::iterator it = CurrentNodes.find( frame );
		if( it == CurrentNodes.end() )
		{
            Node * newNode = CreateNewNode( x, y, pressure );
            int nodeId = m_scene->AddNodeToFrame( newNode, frame );
            CurrentNodes[frame] = nodeId;
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
        m_scene->MarkModified( frameIndex, modifiedRect );
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
        m_scene->MarkModified( frameIndex, modifiedRect );
        ++it;
    }
}

void drwLineTool::MarkOverlaySegmentModified( double x1, double y1, double x2, double y2 )
{
    drwGLRenderer * ren = m_toolbox->GetRenderer();
    if( !ren )
        return;

    double pixSize = ren->UnitsPerPixel();
    double xMin = std::min( x1, x2 );
    double xMax = std::max( x1, x2 );
    double yMin = std::min( y1, y2 );
    double yMax = std::max( y1, y2 );
    double xBoxMin = xMin - m_baseWidth - pixSize;  // grow box by 1 pixel to take into account circle line width
    double xBoxMax = xMax + m_baseWidth + pixSize;
    double yBoxMin = yMin - m_baseWidth - pixSize;
    double yBoxMax = yMax + m_baseWidth + pixSize;
    Box2d modifiedRect( xBoxMin, xBoxMax, yBoxMin, yBoxMax );
    ren->MarkOverlayModified( modifiedRect );
}

void drwLineTool::MarkWholePrimitiveModified()
{
    CurrentNodesCont::iterator it = CurrentNodes.begin();
    while( it != CurrentNodes.end() )
    {
        int nodeId = it->second;
        int frameIndex = it->first;
        Node * n = m_scene->LockNode( frameIndex, nodeId );
        LinePrimitive * prim = dynamic_cast<LinePrimitive*> (n->GetPrimitive());
        Q_ASSERT( prim );
        Box2d modRect = prim->BoundingBox();
        m_scene->MarkModified( frameIndex, modRect );
        m_scene->UnlockNode( frameIndex, nodeId );
        ++it;
    }
}

void drwLineTool::UpdateCursorColor()
{
    if( m_cursor )
    {
        if( !m_erase )
            m_cursor->SetColor( lightGreen );
        else
            m_cursor->SetColor( orange );
        MarkOverlaySegmentModified( m_lastXWorld, m_lastYWorld, m_lastXWorld, m_lastYWorld );
    }
}
