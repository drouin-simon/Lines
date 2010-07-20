#include "drwEditionState.h"

drwEditionState::drwEditionState( QObject * parent )
: QObject( parent )
, m_currentFrame(0)
, m_persistence(0)
, m_frameChangeMode( Manual )
{
}

drwEditionState::~drwEditionState()
{
}

void drwEditionState::SetCurrentFrame( int frame )
{
	m_currentFrame = frame;
	emit CurrentFrameChanged( frame );
}

void drwEditionState::SetPersistence( int nbFrames )
{
	m_persistence = nbFrames;
	emit ModifiedSignal();
}

void drwEditionState::SetFrameChangeMode( drwFrameChangeMode mode )
{
	m_frameChangeMode = mode;
	emit ModifiedSignal();
}

void drwEditionState::Reset()
{
	m_currentFrame = 0;
	m_persistence = 0;
	m_frameChangeMode = Manual;
}