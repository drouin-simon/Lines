#ifndef __drwFpsCounter_h_
#define __drwFpsCounter_h_

#include <QTime>
#include <vector>

class drwFpsCounter
{

public:

    drwFpsCounter();
    void Start();
    void Stop();
    void Tick();
    void SetNumberOfSamples( int nb ) { m_numberOfSamples = nb; }
    double GetFps();
    void SaveTimeStamps( const char * filename );
    bool IsRunning() { return m_isRunning; }

protected:

    int m_numberOfSamples;
    std::vector<int> m_timeStamps;
    QTime m_time;
    bool m_isRunning;

};

#endif
