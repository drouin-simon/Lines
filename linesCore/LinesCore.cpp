#include "LinesCore.h"
#include "Scene.h"
#include "drwGLRenderer.h"
#include "drwToolbox.h"
#include "drwLineTool.h"
#include "drwCommandDispatcher.h"
#include "drwCommandDatabase.h"
#include "drwDrawingSurface.h"

static int defaultNumberOfFrames = 24;

LinesCore::LinesCore()
{
    // initialize playback params
    m_frameInterval = 83;
    m_isPlaying = false;
    m_time.start();

    // Scene
    m_scene = new Scene(this);
    connect( m_scene, SIGNAL(NumberOfFramesChanged(int)), this, SLOT(NumberOfFramesChangedSlot()) );

    // Local toolbox
    m_localToolbox = new drwToolbox( m_scene, this, true );
    connect( m_localToolbox, SIGNAL(FrameChanged()), this, SLOT( FrameChangedSlot() ) );

    // Command db
    m_commandDb = new drwCommandDatabase(this);

    // Command dispatcher
    m_commandDispatcher = new drwCommandDispatcher( m_commandDb, m_localToolbox, m_scene, this );

    // do this after everything else is initialized to make sure we generate a command for the db.
    m_scene->SetNumberOfFrames( defaultNumberOfFrames );

    // GL Renderer
    m_renderer = new drwGLRenderer;
    m_renderer->SetCurrentScene( m_scene );
    m_localToolbox->SetRenderer( m_renderer );
    m_scene->SetRenderer( m_renderer );
}

LinesCore::~LinesCore()
{
    delete m_scene;
    delete m_localToolbox;
    delete m_commandDb;
    delete m_commandDispatcher;
    delete m_renderer;
}

drwLineTool * LinesCore::GetLineTool()
{
    drwLineTool * lineTool = dynamic_cast<drwLineTool*>(m_localToolbox->GetTool( 0 ));
    Q_ASSERT(lineTool);
    return lineTool;
}

void LinesCore::LoadAnimation( const char * filename )
{
    // block signal transmission before reading
    m_renderer->EnableRendering( false );
    m_scene->blockSignals( true );
    m_localToolbox->blockSignals( true );

    // Read commands from the file
    m_commandDb->Read( filename );

    // Dispatch commands
    m_commandDb->LockDb( true );
    for( int i = 0; i < m_commandDb->GetNumberOfCommands(); ++i )
    {
        drwCommand::s_ptr com = m_commandDb->GetCommand( i );
        m_commandDispatcher->IncomingDbCommand( com );
    }
    m_commandDb->LockDb( false );

    // Re-enable signal transmission
    m_localToolbox->blockSignals( false );
    m_scene->blockSignals( false );
    m_renderer->EnableRendering( true );

    m_scene->MarkModified();
}

void LinesCore::SaveAnimation( const char * filename )
{
    m_commandDb->Write( filename );
}

bool LinesCore::IsAnimationModified()
{
    return m_commandDb->IsModified();
}

void LinesCore::Reset()
{
    m_commandDispatcher->Reset();
}

void LinesCore::SetDrawingSurface( drwDrawingSurface * surface )
{
    m_drawingSurface = surface;
    m_renderer->SetDrawingSurface( surface );
}

void LinesCore::SetBackgroundColor( double r, double g, double b, double a )
{
    m_renderer->SetClearColor( r, g, b, a );
}

void LinesCore::SetRenderSize( int w, int h )
{
    m_renderer->SetRenderSize( w, h );
}

void LinesCore::SetShowCursor( bool show )
{
    m_localToolbox->SetShowCursor( show );
}

int LinesCore::GetOnionSkinBefore() { return m_renderer->GetOnionSkinBefore(); }

void LinesCore::SetOnionSkinBefore( int n )
{
    m_renderer->SetOnionSkinBefore( n );
    emit DisplaySettingsModified();
}

int LinesCore::GetOnionSkinAfter() { return m_renderer->GetOnionSkinAfter(); }

void LinesCore::SetOnionSkinAfter( int n )
{
    m_renderer->SetOnionSkinAfter( n );
    emit DisplaySettingsModified();
}

void LinesCore::ReadSettings( QSettings & s ) { m_localToolbox->ReadSettings( s ); }
void LinesCore::WriteSettings( QSettings & s ) { m_localToolbox->WriteSettings( s ); }

void LinesCore::Render()
{
    m_renderer->Render();
}

void LinesCore::MouseEvent( drwMouseCommand::MouseCommandType commandType, double xWin, double yWin, double pressure,
                 int xTilt, int yTilt, double rotation, double tangentialPressure )
{
    double xWorld = 0.0;
    double yWorld = 0.0;
    m_renderer->WindowToWorld( xWin, yWin, xWorld, yWorld );
    drwCommand::s_ptr command( new drwMouseCommand( commandType, xWorld, yWorld, 0.0, xWin, yWin, xTilt, yTilt, pressure, rotation, tangentialPressure ) );
    m_localToolbox->ExecuteCommand( command );
}

void LinesCore::MouseEventWorld( drwMouseCommand::MouseCommandType type, double xWorld, double yWorld, double pressure )
{
    int xWin, yWin;
    m_renderer->WorldToGLWindow( xWorld, yWorld, xWin, yWin );
    drwCommand::s_ptr command( new drwMouseCommand( type, xWorld, yWorld, 0.0, xWin, yWin, 0, 0, pressure, 0, 0 ) );
    m_localToolbox->ExecuteCommand( command );
}

int LinesCore::GetNumberOfFrames()
{
    return m_scene->GetNumberOfFrames();
}

void LinesCore::SetNumberOfFrames( int nb )
{
    m_scene->SetNumberOfFrames( nb );
}

int LinesCore::GetCurrentFrame()
{
    return m_localToolbox->GetCurrentFrame();
}

void LinesCore::SetCurrentFrame( int frame )
{
    m_localToolbox->SetCurrentFrame( frame );
}

void LinesCore::NextFrame()
{
    m_localToolbox->GotoNextFrame();
}

void LinesCore::PrevFrame()
{
    m_localToolbox->GotoPrevFrame();
}

void LinesCore::GotoStart()
{
    SetCurrentFrame(0);
}

void LinesCore::GotoEnd()
{
    SetCurrentFrame( GetNumberOfFrames() - 1 );
}

void LinesCore::SetFrameInterval( int intervalms )
{
    m_frameInterval = intervalms;
}

bool LinesCore::IsPlaying()
{
    return m_isPlaying;
}

void LinesCore::StartPlaying()
{
    if( GetCurrentFrame() == GetNumberOfFrames() - 1 )
        SetCurrentFrame( 0 );
    m_isPlaying = true;
    m_time.restart();
    m_lastFrameWantedTime = 0;
    m_localToolbox->OnStartPlaying();
    m_drawingSurface->NotifyPlaybackStartStop( true );
    emit PlaybackStartStop( true );
}

void LinesCore::StopPlaying()
{
    m_isPlaying = false;
    m_localToolbox->OnStopPlaying();
    m_drawingSurface->NotifyPlaybackStartStop( false );
    emit PlaybackStartStop( false );
}

void LinesCore::PlayPause()
{
    if( m_isPlaying )
    {
        StopPlaying();
    }
    else
    {
        StartPlaying();
    }
}

void LinesCore::Tick()
{
    if( m_isPlaying )
    {
        int current = m_time.elapsed();
        int jump = ( current - m_lastFrameWantedTime ) / m_frameInterval;
        if( jump > 0 )
        {
            int newFrame = GetCurrentFrame() + jump;
            if( newFrame >= GetNumberOfFrames() )
            {
                newFrame = newFrame % GetNumberOfFrames();
            }
            SetCurrentFrame( newFrame );
            m_lastFrameWantedTime += jump * m_frameInterval;
        }
    }
}

void LinesCore::NotifyNeedRender()
{
    m_renderer->NeedRedraw();
}

void LinesCore::EnableRendering( bool enable )
{
    m_renderer->EnableRendering( enable );
}

void LinesCore::SetRemoteIO( drwRemoteCommandIO * io )
{
    m_commandDispatcher->SetRemoteIO( io );
}

int LinesCore::RequestNewUserId()
{
    return m_commandDispatcher->RequestNewUserId();
}

int LinesCore::GetLocalUserId()
{
    return m_commandDispatcher->GetLocalUserId();
}

void LinesCore::IncomingNetCommand( drwCommand::s_ptr com )
{
    m_commandDispatcher->IncomingNetCommand( com );
}

int LinesCore::GetNumberOfDbCommands()
{
    return m_commandDb->GetNumberOfCommands();
}

drwCommand::s_ptr LinesCore::GetDbCommand( int index )
{
    return m_commandDb->GetCommand( index );
}

void LinesCore::LockDb( bool l )
{
    m_commandDb->LockDb( l );
}

void LinesCore::NumberOfFramesChangedSlot()
{
    emit PlaybackSettingsChangedSignal();
}

void LinesCore::FrameChangedSlot()
{
    emit PlaybackSettingsChangedSignal();
}
