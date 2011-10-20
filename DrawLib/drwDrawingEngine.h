#ifndef __drwDrawingEngine_h_
#define __drwDrawingEngine_h_

#include "SVL.h"
#include <QObject>

class drwLineTool;
class drwDrawingWidget;

class drwDrawingEngine : public QObject
{

    Q_OBJECT

public:

    drwDrawingEngine();

    void SetLineTool( drwLineTool * tool ) { m_lineTool = tool; }
    void SetDrawingWidget( drwDrawingWidget * widget ) { m_drawingWidget = widget; }

    void Start();
    void Stop();
    bool IsRunning() { return m_isRunning; }

public slots:

    void Tick();

protected:

    double RandomDouble( double min, double max );
    double RandomDouble();
    int RandomInt( int min, int max );

    drwLineTool * m_lineTool;
    drwDrawingWidget * m_drawingWidget;

    bool m_isRunning;

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
