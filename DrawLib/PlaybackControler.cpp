#include "PlaybackControler.h"
#include "Scene.h"

PlaybackControler::PlaybackControler( Scene * scene, QObject * parent ) 
: QObject( parent )
, m_scene(scene)
, frameInterval(83)  // number of miliseconds between frames
, isPlaying(false)
, loop(true)
, interactionStartFrame(0)
, insertFrameMode(false)
{
    m_backupFrameChangeMode = Manual;
	m_editionState = new drwEditionState( this );
	time.start();
	connect( m_scene, SIGNAL( NumberOfFramesChanged(int) ), this, SLOT( NumberOfFramesChangedSlot( int ) ) );
}


PlaybackControler::~PlaybackControler()
{
	if( isPlaying )
		PlayPause( false );
}


void PlaybackControler::PlayPause( bool manual )
{
	if( isPlaying )
	{
        if( manual )
            m_editionState->SetFrameChangeMode( m_backupFrameChangeMode );
		isPlaying = false;
		emit StartStop( false );
	}
	else
	{
        if( manual )
        {
            m_backupFrameChangeMode = m_editionState->GetFrameChangeMode();
            m_editionState->SetFrameChangeMode( Manual );
        }
		if( GetCurrentFrame() == GetNumberOfFrames() - 1 )
			SetCurrentFrame( 0 );
		isPlaying = true;
		time.restart();
		m_lastFrameWantedTime = 0;
		emit StartStop( true );
	}
}

void PlaybackControler::SetCurrentFrame( int frame )
{
	if( frame != m_editionState->GetCurrentFrame() )
	{
		if( frame >= GetNumberOfFrames() )
			frame = GetNumberOfFrames() - 1;
		else if( frame < 0 )
			frame = 0;
		m_editionState->SetCurrentFrame( frame );
		emit FrameChanged( frame );
		emit ModifiedSignal();
	}
}

int PlaybackControler::GetCurrentFrame() 
{ 
	return m_editionState->GetCurrentFrame(); 
}

int PlaybackControler::GetNextFrame()
{
    int newFrame = GetCurrentFrame() + 1;
    if( newFrame >= GetNumberOfFrames() )
    {
        if( loop )
            newFrame = 0;
    }
    return newFrame;
}

int PlaybackControler::GetNumberOfFrames() 
{ 
	return m_scene->GetNumberOfFrames(); 
}

void PlaybackControler::NumberOfFramesChangedSlot( int nbFrames )
{
	emit ModifiedSignal();
}

void PlaybackControler::NextFrame()
{
    int nextFrame = GetNextFrame();
    if( nextFrame != GetCurrentFrame() )
        SetCurrentFrame( nextFrame );
}


void PlaybackControler::PrevFrame()
{
	int newFrame = GetCurrentFrame() - 1;
	if( newFrame < 0 )
	{
		if( loop )
			SetCurrentFrame( GetNumberOfFrames() - 1 );
	}
	else
	{
		SetCurrentFrame(newFrame);
	}
}

void PlaybackControler::GotoStart()
{
	SetCurrentFrame(0);
}

void PlaybackControler::GotoEnd()
{
	SetCurrentFrame( GetNumberOfFrames() - 1 );
}

void PlaybackControler::SetLooping( bool looping ) 
{ 
	loop = looping;
	emit ModifiedSignal();
}

void PlaybackControler::ToggleLooping() 
{
	SetLooping( !loop );
}

bool PlaybackControler::Tick()
{
	bool didSomething = false;
	if( isPlaying )
	{
		int current = time.elapsed();
		int jump = ( current - m_lastFrameWantedTime ) / frameInterval;
		if( jump > 0 )
		{
			int newFrame = GetCurrentFrame() + jump;
			bool needSetFrame = true;
			if( newFrame >= GetNumberOfFrames() )
			{
				if( loop )
					newFrame = newFrame - GetNumberOfFrames();
				else 
				{
					PlayPause();
					needSetFrame = false;
				}
			}
			if( needSetFrame )
			{
				SetCurrentFrame( newFrame );
				didSomething = true;
				m_lastFrameWantedTime += jump * frameInterval;
			}
		}
	}
	return didSomething;
}


void PlaybackControler::SetFrameInterval( int msecPerFrame )
{
	frameInterval = msecPerFrame;
}


void PlaybackControler::StartInteraction( )
{
	if( m_editionState->GetFrameChangeMode() == Play && !IsPlaying() )
	{
		interactionStartFrame = GetCurrentFrame();
		PlayPause( false );
	}
}


void PlaybackControler::EndInteraction()
{
	if( m_editionState->GetFrameChangeMode() == Play && IsPlaying() )
	{
		PlayPause( false );
		SetCurrentFrame( interactionStartFrame );
	}
	
	if( m_editionState->GetFrameChangeMode() == AfterIntervention && !IsPlaying() )
		NextFrame();
}

