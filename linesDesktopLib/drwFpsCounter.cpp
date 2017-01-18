#include "drwFpsCounter.h"

drwFpsCounter::drwFpsCounter()
    : m_numberOfSamples( 10 )
    , m_isRunning( false )
{
}

void drwFpsCounter::Start()
{
    m_timeStamps.clear();
    m_timeStamps.reserve( 100000 );
    m_time.start();
    m_isRunning = true;
}

void drwFpsCounter::Stop()
{
    m_isRunning = false;
}

void drwFpsCounter::Tick()
{
    int current = m_time.elapsed();
    m_timeStamps.push_back( current );
}

double drwFpsCounter::GetFps()
{
    if( m_timeStamps.size() < 2 )
        return 0.0;

    int numberOfSamples = m_numberOfSamples;
    if( m_timeStamps.size() < m_numberOfSamples )
        numberOfSamples = m_timeStamps.size();
    int sumDiff = 0;
    for( int i = 0; i < numberOfSamples - 1; ++i )
    {
        int time1 = m_timeStamps[ m_timeStamps.size() - i - 2 ];
        int time2 = m_timeStamps[ m_timeStamps.size() - i - 1 ];
        sumDiff += time2 - time1;
    }
    double averageIntervalms = (double)sumDiff / (numberOfSamples - 1);
    double fps = 1000.0 / averageIntervalms;
    return fps;
}

void drwFpsCounter::SaveTimeStamps( const char * filename )
{

}
