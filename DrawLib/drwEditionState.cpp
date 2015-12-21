#include "drwEditionState.h"
#include <QSettings>

drwEditionState::drwEditionState( QObject * parent )
: QObject( parent )
, m_currentFrame(0)
, m_frameChangeMode( Manual )
, m_persistence( 0 )
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

void drwEditionState::SetFrameChangeMode( drwFrameChangeMode mode )
{
	m_frameChangeMode = mode;
	emit ModifiedSignal();
}

void drwEditionState::SetPersistence( int nbFrames )
{
    m_persistence = nbFrames;
    emit ModifiedSignal();
}

void drwEditionState::Reset()
{
	m_currentFrame = 0;
    m_frameChangeMode = Manual;
    m_persistence = 4;
    emit ModifiedSignal();
}

void drwEditionState::ReadSettings( QSettings & s )
{
    m_persistence = s.value( "Persistence", QVariant(m_persistence) ).toInt();
    emit ModifiedSignal();
}

void drwEditionState::WriteSettings( QSettings & s )
{
    s.setValue( "Persistence", QVariant(m_persistence) );
}
