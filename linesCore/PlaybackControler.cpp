#include "PlaybackControler.h"
#include "Scene.h"
#include "drwToolbox.h"

PlaybackControler::PlaybackControler( Scene * scene )
: m_scene(scene)
, frameInterval(83)  // number of miliseconds between frames
, isPlaying(false)
, loop(true)
{
	time.start();
	connect( m_scene, SIGNAL( NumberOfFramesChanged(int) ), this, SLOT( NumberOfFramesChangedSlot( int ) ) );
}

PlaybackControler::~PlaybackControler()
{
	if( isPlaying )
        StopPlaying();
}

void PlaybackControler::SetToolbox( drwToolbox * t )
{
    m_toolbox = t;
}

void PlaybackControler::SetPlaying( bool play )
{
    if( IsPlaying() == play )
        return;
    if( play )
        StartPlaying();
    else
        StopPlaying();
}

void PlaybackControler::StartPlaying()
{
    if( GetCurrentFrame() == GetNumberOfFrames() - 1 )
        SetCurrentFrame( 0 );
    isPlaying = true;
    time.restart();
    m_lastFrameWantedTime = 0;
    m_toolbox->OnStartPlaying();
    emit StartStop( true );
}

void PlaybackControler::StopPlaying()
{
    isPlaying = false;
    m_toolbox->OnStopPlaying();
    emit StartStop( false );
}

void PlaybackControler::PlayPause()
{
	if( isPlaying )
	{
        StopPlaying();
	}
	else
	{
        StartPlaying();
	}
}

void PlaybackControler::SetCurrentFrame( int frame )
{
    m_toolbox->SetCurrentFrame( frame );
}

void PlaybackControler::NotifyFrameChanged()
{
    emit ModifiedSignal();
}

int PlaybackControler::GetCurrentFrame() 
{ 
    return m_toolbox->GetCurrentFrame();
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
                    newFrame = newFrame % GetNumberOfFrames();
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


