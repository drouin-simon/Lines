#include "drwToolbox.h"
#include "drwLineTool.h"
#include "drwGLRenderer.h"
#include "Scene.h"
#include "LinesCore.h"
#include <cassert>

drwToolbox::drwToolbox( Scene * scene, LinesCore * lc, bool local )
: m_scene( scene ), m_lines( lc ), m_renderer( 0 ), m_isLocal( local )
{
    drwLineTool * drawTool = new drwLineTool( m_scene, this );
	AddTool( drawTool );
    m_currentTool = 0;
    m_currentFrame = 0;
}

drwToolbox::~drwToolbox()
{
	for( unsigned i = 0; i < Tools.size(); ++i )
		delete Tools[i];
}

void drwToolbox::SetRenderer( drwGLRenderer * r )
{
    m_renderer = r;
    for( unsigned i = 0; i < Tools.size(); ++i )
        Tools[i]->NotifyRendererChanged();
}

void drwToolbox::AddTool( drwTool * tool )
{
	connect( tool, SIGNAL(CommandExecuted(drwCommand::s_ptr)), SLOT(NotifyCommandExecuted(drwCommand::s_ptr)) );
	Tools.push_back( tool );
}

drwTool * drwToolbox::GetTool( int index )
{
	if( index >= 0 && index < Tools.size() )
		return Tools[ index ];
	return 0;
}

void drwToolbox::ReadSettings( QSettings & s )
{
    for( unsigned i = 0; i < Tools.size(); ++i )
        Tools[i]->ReadSettings( s );
}

void drwToolbox::WriteSettings( QSettings & s )
{
    for( unsigned i = 0; i < Tools.size(); ++i )
        Tools[i]->WriteSettings( s );
}

void drwToolbox::StartPlaying()
{
    if( m_lines )
        m_lines->StartPlaying();
}

bool drwToolbox::IsPlaying()
{
    if( m_lines )
        return m_lines->IsPlaying();
    return false;
}

void drwToolbox::OnStartPlaying()
{
    for( unsigned i = 0; i < Tools.size(); ++i )
        Tools[i]->OnStartPlaying();
}

void drwToolbox::StopPlaying()
{
    if( m_lines )
        m_lines->StopPlaying();
}

void drwToolbox::OnStopPlaying()
{
    for( unsigned i = 0; i < Tools.size(); ++i )
        Tools[i]->OnStopPlaying();
}

void drwToolbox::SetCurrentFrame( int frame )
{
    assert( frame < GetNumberOfFrames() );
    assert( frame >= 0 );
    if( frame == m_currentFrame )
        return;

    m_currentFrame = frame;

    // Tell current tool the frame has changed
    if( m_currentTool >= 0 && m_currentTool < Tools.size() )
        Tools[m_currentTool]->NotifyFrameChanged( frame );

    emit FrameChanged();

    if( m_renderer )
        m_renderer->SetRenderFrame( frame );
	
    // Create a command to mark the frame has changed
	drwSetFrameCommand * com = new drwSetFrameCommand();
	com->SetNewFrame(frame);
	drwCommand::s_ptr command( com );
	emit CommandExecuted( command );
}

void drwToolbox::GotoNextFrame()
{
    int nextFrame = ( m_currentFrame + 1 ) % GetNumberOfFrames();
    SetCurrentFrame( nextFrame );
}

void drwToolbox::GotoPrevFrame()
{
    int prevFrame = m_currentFrame == 0 ? GetNumberOfFrames() - 1 : m_currentFrame - 1;
    SetCurrentFrame( prevFrame );
}

int drwToolbox::GetNumberOfFrames()
{
    return m_scene->GetNumberOfFrames();
}

void drwToolbox::EmitStateCommand()
{
	for( unsigned i = 0; i < Tools.size(); ++i )
        Tools[i]->EmitStateCommand();
}

void drwToolbox::PostAnimationLoad()
{
    for( unsigned i = 0; i < Tools.size(); ++i )
        Tools[i]->PostAnimationLoad();
}

void drwToolbox::blockSignals( bool block )
{
	QObject::blockSignals( block );
	for( unsigned i = 0; i < Tools.size(); ++i )
		Tools[i]->blockSignals( block );
}

void drwToolbox::NotifyCommandExecuted( drwCommand::s_ptr command )
{
	emit CommandExecuted( command );
}

void drwToolbox::ExecuteCommand( drwCommand::s_ptr command )
{
	if( command->GetCommandId() == drwIdSetFrameCommand )
	{
		drwSetFrameCommand * setFrameCom = dynamic_cast<drwSetFrameCommand*> (command.get());
		SetCurrentFrame( setFrameCom->GetNewFrame() );
	}
    else if( m_currentTool >= 0 && m_currentTool < Tools.size() )
        Tools[m_currentTool]->ExecuteCommand( command );
}

void drwToolbox::SetShowCursor( bool show )
{
    for( unsigned i = 0; i < Tools.size(); ++i )
        Tools[i]->SetShowCursor( show );
}
