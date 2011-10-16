#include "drwDrawingEngine.h"
#include "drwLineTool.h"

const double drwDrawingEngine::m_maxAngularAcceleration = 0.005;
const double drwDrawingEngine::m_stepSize = 10.0;

drwDrawingEngine::drwDrawingEngine()
    : m_nbPointsInSegment( 0 )
    , m_lastPoint( Vec2( 0, 0 ) )
    , m_lastAngle( 0.0 )
    , m_lastAngularSpeed( 0.0 )
    , m_lastAngularAcceleration( 0.0 )
{
}

void drwDrawingEngine::Start()
{

}

void drwDrawingEngine::Stop()
{

}

void drwDrawingEngine::Tick()
{
    // Start a new segment if needed
    if( --m_nbPointsInSegment < 0 )
    {
        m_nbPointsInSegment = RandomInt( 5, 1000 );
        m_lastPoint = Vec2( RandomDouble(200, 1800), RandomDouble(200, 800) );
        m_lastAngle = RandomDouble( 0, 2 * vl_pi );
        m_lastAngularSpeed = 0.0;
        m_lastAngularAcceleration = 0.0;
        m_segmentColor = RandomDouble( 0.2, 1.0 );
        Vec4 col( m_segmentColor, m_segmentColor, m_segmentColor, 1.0 );
        m_lineTool->SetColor( col );

        m_lineTool->StartLine( m_lastPoint[0], m_lastPoint[1] );
    }
    else
    {
        m_lastAngularAcceleration += RandomDouble( -m_maxAngularAcceleration, m_maxAngularAcceleration );
        m_lastAngularSpeed += m_lastAngularAcceleration;
        m_lastAngle += m_lastAngularSpeed;
        Vec2 direction( cos( m_lastAngle), sin(m_lastAngle) );
        m_lastPoint += direction * m_stepSize;

        if( m_nbPointsInSegment != 0 )
        {
            m_lineTool->AddPoint( m_lastPoint[0], m_lastPoint[1] );
        }
        else
        {
            m_lineTool->EndLine( m_lastPoint[0], m_lastPoint[1] );
        }

    }
}

// this could be changed in the future for more sofisticated random
// number generator. For now, it will generate a random number in
// the range [0,1].
double drwDrawingEngine::RandomDouble()
{
    return vl_rand();
}

double drwDrawingEngine::RandomDouble( double min, double max )
{
    double ztoo = RandomDouble();
    double res = min + (max - min) * ztoo;
    return res;
}

int drwDrawingEngine::RandomInt( int min, int max )
{
    int diff = max - min;
    int nb = rand();
    int offset = nb % diff;
    return min + offset;
}
