#ifndef __DrawingWidgetMT_h_
#define __DrawingWidgetMT_h_

#include <QOpenGLWidget>
#include <QThread>
#include "drwCommand.h"

class DrawingWidgetMT;
class LinesCore;

class RenderThread : public QThread
{
    Q_OBJECT
public:

    RenderThread();
    virtual ~RenderThread();
    void SetDrawingWidget( DrawingWidgetMT * dw );
    void SetLinesCore( LinesCore * lines );

protected:

    virtual void run();

    DrawingWidgetMT * m_drawingWidget;
    LinesCore * m_lines;
};

class DrawingWidgetMT : public QOpenGLWidget
{

    Q_OBJECT

public:

    DrawingWidgetMT( LinesCore * lc, QWidget * parent = 0 );
    virtual ~DrawingWidgetMT();

    void NotifyPlaybackStartStop( bool isStart );

    void SetMuteMouse( bool mute ) { m_muteMouse = mute; }
    bool IsMutingMouse() { return m_muteMouse; }

protected slots:

    void startCompositing();
    void finishCompositing();
    void StartResize();
    void FinishResize();

protected:

    // paintEvent does nothing as the painting is done in the render thread
    virtual void paintEvent( QPaintEvent * ) override {};
    virtual void initializeGL() override;

    void tabletEvent( QTabletEvent * ) override;
    virtual void mousePressEvent( QMouseEvent * ) override;
    virtual void mouseReleaseEvent( QMouseEvent * ) override;
    virtual void mouseMoveEvent( QMouseEvent * ) override;
    void enterEvent( QEvent * ) override;
    void leaveEvent( QEvent * ) override;
    bool event ( QEvent * ) override;

    void tryRender();

    // Convert mouse and tablet event to lines command and push them
    void MouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e );
    void MouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e );

    RenderThread * m_renderThread;
    LinesCore * m_lines;
    bool m_drawing;

    // Manage timer that generates updateGL in playback mode
    int m_timerId;

    // some drivers are weird, it may be necessary to ignore mouse events
    bool m_muteMouse;

    // make sur not to generate both mouse and tablet events
    bool m_tabletHasControl;


};

#endif
