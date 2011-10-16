#ifndef __drwDrawingEngine_h_
#define __drwDrawingEngine_h_

#include "SVL.h"

class drwLineTool;

class drwDrawingEngine
{

public:

    drwDrawingEngine();

    void SetLineTool( drwLineTool * tool ) { m_lineTool = tool; }

    void Start();
    void Stop();
    void Tick();

protected:

    double RandomDouble( double min, double max );
    double RandomDouble();
    int RandomInt( int min, int max );

    drwLineTool * m_lineTool;

    int m_nbPointsInSegment;
    double m_segmentColor;
    Vec2 m_lastPoint;
    double m_lastAngle;
    double m_lastAngularSpeed;
    double m_lastAngularAcceleration;

    static const double m_stepSize;
    static const double m_maxAngularAcceleration;

};

#endif
