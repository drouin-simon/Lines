#include "LinesCore.h"
#include "Scene.h"
#include "drwGLRenderer.h"
#include "drwToolbox.h"
#include "drwLineTool.h"
#include "drwCommandDatabase.h"
#include "drwDrawingSurface.h"
#include "drwRemoteCommandIO.h"

static int defaultNumberOfFrames = 24;
const int LinesCore::m_localToolboxId = 0;

LinesCore::LinesCore()
{
    // initialize params
    m_frameInterval = 83;
    m_isPlaying = false;
    m_time.start();
    m_lastUsedUserId = 0;
    m_remoteIO = 0;
    m_commandsPassThrough = false;

    // Scene
    m_scene = new Scene(this);
    connect( m_scene, SIGNAL(NumberOfFramesChanged(int)), this, SLOT(NumberOfFramesChangedSlot()) );
    connect( m_scene, SIGNAL(CommandExecuted(drwCommand::s_ptr)), this, SLOT(IncomingLocalCommand(drwCommand::s_ptr)), Qt::DirectConnection );

    // Local toolbox
    m_localToolbox = new drwToolbox( m_scene, this, true );
    m_toolboxes[ m_localToolboxId ] = m_localToolbox;
    connect( m_localToolbox, SIGNAL(FrameChanged()), this, SLOT( FrameChangedSlot() ), Qt::DirectConnection );
    connect( m_localToolbox, SIGNAL(CommandExecuted(drwCommand::s_ptr)), this, SLOT(IncomingLocalCommand(drwCommand::s_ptr)), Qt::DirectConnection );

    // Command db
    m_commandDb = new drwCommandDatabase(this);

    // do this after everything else is initialized to make sure we generate a command for the db.
    m_scene->SetNumberOfFrames( defaultNumberOfFrames );

    // GL Renderer
    m_renderer = new drwGLRenderer;
    m_renderer->SetCurrentScene( m_scene );
    m_localToolbox->SetRenderer( m_renderer );
    m_scene->SetRenderer( m_renderer );
    m_stackedCurrentFrame = m_localToolbox->GetCurrentFrame();
}

LinesCore::~LinesCore()
{
    delete m_scene;
    foreach( drwToolbox * current, m_toolboxes )
    {
        delete current;
    }
    delete m_commandDb;
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
        IncomingDbCommand( com );
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
    // todo : This should probably be done in network manager
    if( m_remoteIO->IsSharing() )
    {
        drwCommand::s_ptr newSceneCommand( new drwNewSceneCommand );
        m_remoteIO->SendCommand( newSceneCommand );
    }
    m_scene->Clear();
    m_commandDb->Clear();
    ClearAllToolboxesButLocal();
    m_localToolbox->Reset();
    m_lastUsedUserId = 0;
    m_cachedStateCommands.clear();
    m_stackedCurrentFrame = m_localToolbox->GetCurrentFrame();
}

void LinesCore::BeginCommandStream()
{
    m_commandsPassThrough = true;
}

void LinesCore::EndCommandStream()
{
    m_commandsPassThrough = false;
    m_drawingSurface->WaitRenderFinished();
    m_drawingSurface->Render();
}

void LinesCore::SetLineErase( bool erase )
{
    BeginChangeToolParams();
    GetLineTool()->SetErase( erase );
    EndChangeToolParams();
}

void LinesCore::SetLineBaseWidth( double w )
{
    BeginChangeToolParams();
    GetLineTool()->SetBaseWidth( w );
    EndChangeToolParams();
}

void LinesCore::SetLineColor( Vec4 color )
{
    BeginChangeToolParams();
    GetLineTool()->SetColor( color );
    EndChangeToolParams();
}

void LinesCore::SetLinePressureWidth( bool pw )
{
    BeginChangeToolParams();
    GetLineTool()->SetPressureWidth( pw );
    EndChangeToolParams();
}

void LinesCore::SetLinePressureOpacity( bool po )
{
    BeginChangeToolParams();
    GetLineTool()->SetPressureOpacity( po );
    EndChangeToolParams();
}

void LinesCore::SetLineFill( bool f )
{
    BeginChangeToolParams();
    GetLineTool()->SetFill( f );
    EndChangeToolParams();
}

void LinesCore::SetLinePersistence( int p )
{
    BeginChangeToolParams();
    GetLineTool()->SetPersistence( p );
    EndChangeToolParams();
}

void LinesCore::SetDrawingSurface( drwDrawingSurface * surface )
{
    m_drawingSurface = surface;
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
    m_drawingSurface->WaitRenderFinished();
    m_localToolbox->SetShowCursor( show );
    m_drawingSurface->Render();
}

int LinesCore::GetOnionSkinBefore() { return m_renderer->GetOnionSkinBefore(); }

void LinesCore::SetOnionSkinBefore( int n )
{
    m_drawingSurface->WaitRenderFinished();
    m_renderer->SetOnionSkinBefore( n );
    m_drawingSurface->Render();
    emit DisplaySettingsModified();
}

int LinesCore::GetOnionSkinAfter() { return m_renderer->GetOnionSkinAfter(); }

void LinesCore::SetOnionSkinAfter( int n )
{
    m_drawingSurface->WaitRenderFinished();
    m_renderer->SetOnionSkinAfter( n );
    m_drawingSurface->Render();
    emit DisplaySettingsModified();
}

void LinesCore::ReadSettings( QSettings & s ) { m_localToolbox->ReadSettings( s ); }
void LinesCore::WriteSettings( QSettings & s ) { m_localToolbox->WriteSettings( s ); }

bool LinesCore::NeedsRender()
{
    // todo: check if this sync is write. It seems like some repaint requests may be lost.
    m_localCommandsMutex.lock();
    bool res = m_localCommandsToProcess.size() > 0;
    m_localCommandsMutex.unlock();
    m_netCommandsMutex.lock();
    res |= m_netCommandsToProcess.size() > 0;
    m_netCommandsMutex.unlock();
    return res;
}

void LinesCore::Render()
{
    ExecuteLocalCommands();
    ExecuteNetCommands();
    m_renderer->Render();
}

void LinesCore::MouseEvent( drwMouseCommand::MouseCommandType commandType, double xWin, double yWin, double pressure,
                 int xTilt, int yTilt, double rotation, double tangentialPressure )
{
    double xWorld = 0.0;
    double yWorld = 0.0;
    m_renderer->WindowToWorld( xWin, yWin, xWorld, yWorld );
    drwCommand::s_ptr command( new drwMouseCommand( commandType, xWorld, yWorld, 0.0, xWin, yWin, xTilt, yTilt, pressure, rotation, tangentialPressure ) );
    PushLocalCommand( command );
}

void LinesCore::MouseEventWorld( drwMouseCommand::MouseCommandType type, double xWorld, double yWorld, double pressure )
{
    int xWin, yWin;
    m_renderer->WorldToGLWindow( xWorld, yWorld, xWin, yWin );
    drwCommand::s_ptr command( new drwMouseCommand( type, xWorld, yWorld, 0.0, xWin, yWin, 0, 0, pressure, 0, 0 ) );
    PushLocalCommand( command );
}

int LinesCore::GetNumberOfFrames()
{
    return m_scene->GetNumberOfFrames();
}

void LinesCore::SetNumberOfFrames( int nb )
{
    m_drawingSurface->WaitRenderFinished();
    m_scene->SetNumberOfFrames( nb );
    m_drawingSurface->Render();
}

int LinesCore::GetCurrentFrame()
{
    return m_stackedCurrentFrame;
}

void LinesCore::SetCurrentFrame( int frame )
{
    m_stackedCurrentFrame = frame;
    drwSetFrameCommand * com = new drwSetFrameCommand();
    com->SetNewFrame(frame);
    drwCommand::s_ptr command( com );
    PushLocalCommand( command );
}

void LinesCore::NextFrame()
{
    int nextFrame = ( GetCurrentFrame() + 1 ) % GetNumberOfFrames();
    SetCurrentFrame( nextFrame );
}

void LinesCore::PrevFrame()
{
    int prevFrame = GetCurrentFrame() == 0 ? GetNumberOfFrames() - 1 : GetCurrentFrame() - 1;
    SetCurrentFrame( prevFrame );
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
    m_renderer->SetInhibitOnionSkin( true );
    m_localToolbox->OnStartPlaying();
    m_drawingSurface->NotifyPlaybackStartStop( true );
    emit PlaybackStartStop( true );
}

void LinesCore::StopPlaying()
{
    m_isPlaying = false;
    m_renderer->SetInhibitOnionSkin( false );
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

void LinesCore::SetRemoteIO( drwRemoteCommandIO * io )
{
    m_remoteIO = io;
}

int LinesCore::RequestNewUserId()
{
    return ++m_lastUsedUserId;
}

int LinesCore::GetLocalUserId()
{
    return m_localToolboxId;
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

void LinesCore::IncomingNetCommand( drwCommand::s_ptr com )
{
    PushNetCommand( com );
}

void LinesCore::IncomingLocalCommand( drwCommand::s_ptr command )
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
            m_commandDb->PushCommand( m_cachedStateCommands[i] );
        }
        m_cachedStateCommands.clear();

        // Send it to the network
        if( m_remoteIO )
            m_remoteIO->SendCommand( command );

        // Store it in the database
        m_commandDb->PushCommand( command );
    }
}

void LinesCore::IncomingDbCommand( drwCommand::s_ptr command )
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

void LinesCore::NumberOfFramesChangedSlot()
{
    emit PlaybackSettingsChangedSignal();
}

void LinesCore::FrameChangedSlot()
{
    emit PlaybackSettingsChangedSignal();
}

drwToolbox * LinesCore::AddUser( int commandUserId )
{
    drwToolbox * newUser = new drwToolbox( m_scene, NULL, false );
    m_toolboxes[ commandUserId ] = newUser;
    if( m_lastUsedUserId < commandUserId )
        m_lastUsedUserId = commandUserId;
    return newUser;
}

void LinesCore::ClearAllToolboxesButLocal()
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

void LinesCore::BeginChangeToolParams()
{
    m_drawingSurface->WaitRenderFinished();
    ExecuteLocalCommands();
}

void LinesCore::EndChangeToolParams()
{
    m_drawingSurface->Render();
}

void LinesCore::PushLocalCommand( drwCommand::s_ptr com )
{
    m_localCommandsMutex.lock();
    m_localCommandsToProcess.push_back( com );
    m_localCommandsMutex.unlock();
}

void LinesCore::ExecuteLocalCommands()
{
    m_localCommandsMutex.lock();
    foreach( drwCommand::s_ptr com, m_localCommandsToProcess )
    {
        m_localToolbox->ExecuteCommand( com );
    }
    m_localCommandsToProcess.clear();
    m_localCommandsMutex.unlock();
}

void LinesCore::PushNetCommand( drwCommand::s_ptr com )
{
    if( !m_commandsPassThrough )
    {
        m_netCommandsMutex.lock();
        m_netCommandsToProcess.push_back( com );
        m_netCommandsMutex.unlock();
        m_drawingSurface->PostRender();
    }
    else
        ExecuteOneNetCommand( com );
}

void LinesCore::ExecuteNetCommands()
{
    m_netCommandsMutex.lock();
    foreach( drwCommand::s_ptr com, m_netCommandsToProcess )
    {
        ExecuteOneNetCommand( com );
    }
    m_netCommandsToProcess.clear();
    m_netCommandsMutex.unlock();
}

void LinesCore::ExecuteOneNetCommand( drwCommand::s_ptr com )
{
    if( com->GetCommandId() == drwIdNewSceneCommand )
    {
        Reset();
    }
    else if( com->GetCommandId() == drwIdSceneParamsCommand )
    {
        drwSceneParamsCommand * serverMsg = dynamic_cast<drwSceneParamsCommand*> (com.get());
        m_scene->SetNumberOfFrames( serverMsg->GetNumberOfFrames() );

        // Store it in the database
        m_commandDb->PushCommand( com );
    }
    else
    {
        // Execute the command in the appropriate toolbox
        int commandUserId = com->GetUserId();
        drwToolbox * box = m_toolboxes[ commandUserId ];
        if( !box )
            box = AddUser( commandUserId );
        box->ExecuteCommand( com );

        // Store it in the database
        m_commandDb->PushCommand( com );
    }
}
