#ifndef __drwDrawingEngine_h_
#define __drwDrawingEngine_h_

#include "SVL.h"
#include <QObject>

class drwLineTool;

class drwDrawingEngine : public QObject
{

    Q_OBJECT

public:

    drwDrawingEngine();

    void SetLineTool( drwLineTool * tool ) { m_lineTool = tool; }

    void Start();
    void Stop();
    bool IsRunning() { return m_timerId != -1; }

protected:

    virtual void timerEvent( QTimerEvent * event );

    double RandomDouble( double min, double max );
    double RandomDouble();
    int RandomInt( int min, int max );

    int m_timerId;

    drwLineTool * m_lineTool;

    int m_nbPointsInSegment;
    double m_segmentColor;
    Vec2 m_lastPoint;
    double m_lastAngle;
    double m_lastAngularSpeed;
    double m_lastAngularAcceleration;

    static const double m_stepSize;
    static const double m_maxAngularAcceleration;
    static const int m_minNumberOfPoints;
    static const int m_maxNumberOfPoints;

};

#endif
