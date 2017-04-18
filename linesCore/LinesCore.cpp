#include "LinesCore.h"
#include "Scene.h"
#include "drwGLRenderer.h"
#include "drwToolbox.h"
#include "drwLineTool.h"
#include "PlaybackControler.h"
#include "drwCommandDispatcher.h"
#include "drwCommandDatabase.h"
#include "drwDrawingSurface.h"

static int defaultNumberOfFrames = 24;

LinesCore::LinesCore()
{
    m_scene = new Scene(this);
    m_controler = new PlaybackControler( m_scene );
    m_localToolbox = new drwToolbox( m_scene, m_controler );
    m_controler->SetToolbox( m_localToolbox );
    connect( m_controler, SIGNAL(ModifiedSignal()), this, SLOT(PlaybackSettingsChangedSlot()) );
    m_commandDb = new drwCommandDatabase(this);
    m_commandDispatcher = new drwCommandDispatcher( m_commandDb, m_localToolbox, m_scene, this );
    m_scene->SetNumberOfFrames( defaultNumberOfFrames ); // do this after everything else is initialized to make sure we generate a command for the db.
    m_renderer = new drwGLRenderer;
    m_renderer->SetCurrentScene( m_scene );
    m_localToolbox->SetRenderer( m_renderer );
    m_scene->SetRenderer( m_renderer );
}

LinesCore::~LinesCore()
{
    delete m_scene;
    delete m_controler;
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
    m_renderer->SetShowCursor( show );
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
    return m_controler->GetCurrentFrame();
}

void LinesCore::SetCurrentFrame( int frame )
{
    m_controler->SetCurrentFrame( frame );
}

void LinesCore::NextFrame()
{
    m_controler->NextFrame();
}

void LinesCore::PrevFrame()
{
    m_controler->PrevFrame();
}

void LinesCore::GotoStart()
{
    m_controler->GotoStart();
}

void LinesCore::GotoEnd()
{
    m_controler->GotoEnd();
}

void LinesCore::SetFrameInterval( int intervalms )
{
    m_controler->SetFrameInterval( intervalms );
}

bool LinesCore::IsLooping()
{
    return m_controler->GetLooping();
}

void LinesCore::SetLooping( bool loop )
{
    m_controler->SetLooping( loop );
}

bool LinesCore::IsPlaying()
{
    return m_controler->IsPlaying();
}

void LinesCore::PlayPause()
{
    m_controler->PlayPause();
}

void LinesCore::Tick()
{
    m_controler->Tick();
}

void LinesCore::NotifyNeedRender()
{
    m_renderer->NeedRedraw();
}

void LinesCore::EnableRendering( bool enable )
{
    m_scene->blockSignals( !enable );
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

void LinesCore::PlaybackStartStop( bool isStarting )
{
    m_drawingSurface->NotifyPlaybackStartStop( isStarting );
}

void LinesCore::PlaybackSettingsChangedSlot()
{
    emit PlaybackSettingsChangedSignal();
}
