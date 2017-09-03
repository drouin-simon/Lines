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
    m_onionSkinEnabled = true;
    m_remoteIO = 0;

    // Flip frame mode
    m_flipFramesMode = false;
    m_flippingFrames = false;
    m_flipStartY = 0.0;
    m_flipStartFrame = 0;

    // Scene
    m_scene = new Scene(this);
    connect( m_scene, SIGNAL(NumberOfFramesChanged(int)), this, SLOT(NumberOfFramesChangedSlot()) );
    connect( m_scene, SIGNAL(CommandExecuted(drwCommand::s_ptr)), this, SLOT(IncomingLocalCommand(drwCommand::s_ptr)));

    // Local toolbox
    m_localToolbox = new drwToolbox( m_scene, this, true );
    m_toolboxes[ m_localToolboxId ] = m_localToolbox;
    connect( m_localToolbox, SIGNAL(FrameChanged()), this, SLOT( FrameChangedSlot() ) );
    connect( m_localToolbox, SIGNAL(CommandExecuted(drwCommand::s_ptr)), this, SLOT(IncomingLocalCommand(drwCommand::s_ptr)));

    // Command db
    m_commandDb = new drwCommandDatabase(this);

    // do this after everything else is initialized to make sure we generate a command for the db.
    m_scene->SetNumberOfFrames( defaultNumberOfFrames );

    // GL Renderer
    m_renderer = new drwGLRenderer;
    m_renderer->SetCurrentScene( m_scene );
    m_localToolbox->SetRenderer( m_renderer );
    m_scene->SetRenderer( m_renderer );

    // Mechanism to exectute net commands (coming from different threads) into the main thread
    connect( this, SIGNAL(NetCommandAvailable()), this, SLOT(ProcessPendingNetCommandsSlot()), Qt::QueuedConnection );
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

    SetCurrentFrame(0);
    ClearAnimation();

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

    m_localToolbox->PostAnimationLoad();
    m_scene->MarkModified();

    // Tell ui everything is changed (make sure all ui is updated)
    emit DisplaySettingsModified();
    emit PlaybackSettingsChangedSignal();
}

void LinesCore::SaveAnimation( const char * filename )
{
    m_commandDb->Write( filename );
}

bool LinesCore::IsAnimationModified()
{
    return m_commandDb->IsModified();
}

void LinesCore::NewAnimation()
{
    // Tell clients to clear their animation
    if( m_remoteIO->IsSharing() )
    {
        drwCommand::s_ptr newSceneCommand( new drwNewSceneCommand );
        m_remoteIO->SendCommand( newSceneCommand );
    }

    // Clear local animation
    ClearAnimation();

    // Remember initial state in the new animation
    EmitStateCommands();

    // Restart at frame 0
    SetCurrentFrame( 0 );
}

void LinesCore::EmitStateCommands()
{
    m_scene->EmitStateCommand();
    m_localToolbox->EmitStateCommand();
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

bool LinesCore::GetOnionSkinEnabled()
{
    return m_onionSkinEnabled;
}

void LinesCore::SetOnionSkinEnabled( bool e )
{
    m_onionSkinEnabled = e;
    if( !IsPlaying() )
        m_renderer->SetInhibitOnionSkin( !e );
    emit DisplaySettingsModified();
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

void LinesCore::ToggleInOnionFrame()
{
    if( m_renderer->GetInOnionFrame() == -1 )
        m_renderer->SetInOnionFrame( GetCurrentFrame() );
    else
        m_renderer->SetInOnionFrame( -1 );
}

void LinesCore::ToggleOutOnionFrame()
{
    if( m_renderer->GetOutOnionFrame() == -1 )
        m_renderer->SetOutOnionFrame( GetCurrentFrame() );
    else
        m_renderer->SetOutOnionFrame( -1 );
}

bool LinesCore::IsFlippingFrameModeEnabled()
{
    return m_flipFramesMode;
}

void LinesCore::SetFlippingFrameModeEnabled( bool enable )
{
    m_flipFramesMode = enable;
    SetOnionSkinEnabled( !m_flipFramesMode );
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
    if( m_flipFramesMode || m_flippingFrames /* haven't finished*/ )
    {
        HandleFlipFrameEvent( commandType, yWin );
    }
    else
    {
        double xWorld = 0.0;
        double yWorld = 0.0;
        m_renderer->WindowToWorld( xWin, yWin, xWorld, yWorld );
        drwCommand::s_ptr command( new drwMouseCommand( commandType, xWorld, yWorld, 0.0, xWin, yWin, xTilt, yTilt, pressure, rotation, tangentialPressure ) );
        m_localToolbox->ExecuteCommand( command );
    }
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
    emit PlaybackSettingsChangedSignal();
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
    m_renderer->SetInhibitOnionSkin( !m_onionSkinEnabled );
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
    m_netCommandsMutex.lock();
    m_netCommandsToProcess.push_back( com );
    m_netCommandsMutex.unlock();

    emit NetCommandAvailable();
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

void LinesCore::ProcessPendingNetCommandsSlot()
{
    // Execute pending commands
    ExecuteNetCommands();
}

drwToolbox * LinesCore::AddUser( int commandUserId )
{
    drwToolbox * newUser = new drwToolbox( m_scene, NULL, false );
    m_toolboxes[ commandUserId ] = newUser;
    if( m_lastUsedUserId < commandUserId )
        m_lastUsedUserId = commandUserId;
    return newUser;
}

void LinesCore::ClearAnimation()
{
    m_scene->Clear();
    m_commandDb->Clear();
    ClearAllToolboxesButLocal();
    m_lastUsedUserId = 0;
    m_cachedStateCommands.clear();
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

void LinesCore::HandleFlipFrameEvent( drwMouseCommand::MouseCommandType commandType, double yWin )
{
    // We hardcode 3% screen height = move one frame. Todo: do something smarter
    double ratioScreenPerFrame = 0.03;
    int screenHeight = m_renderer->GetRenderSize()[1];
    int nbPixPerFrame = (int)( ratioScreenPerFrame * screenHeight );

    if( commandType == drwMouseCommand::Press )
    {
        m_flippingFrames = true;
        m_flipStartY = (int)yWin;
        m_flipStartFrame = GetCurrentFrame();
    }
    else if( commandType == drwMouseCommand::Release )
    {
        m_flippingFrames = false;
        SetCurrentFrame( m_flipStartFrame );
    }
    else if( commandType == drwMouseCommand::Move && m_flippingFrames )
    {
        int newFrame = m_flipStartFrame - ( (int)yWin - m_flipStartY ) / nbPixPerFrame;
        if( newFrame >= GetNumberOfFrames() )
            newFrame = newFrame % GetNumberOfFrames();
        if( newFrame < 0 )
            newFrame = GetNumberOfFrames() - ( std::abs( newFrame ) % GetNumberOfFrames() );
        SetCurrentFrame( newFrame );
    }
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
        NewAnimation();
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
