#ifndef __TabletStateTestingArea_h_
#define __TabletStateTestingArea_h_

#include <QOpenGLWidget>

class TabletStateWidget;

class TabletStateTestingArea : public QOpenGLWidget
{
    Q_OBJECT

public:

    explicit TabletStateTestingArea(QWidget *parent = 0);

    void SetTabletStateWidget( TabletStateWidget * w ) { m_parent = w; }

protected:

    virtual void initializeGL();
    virtual void resizeGL( int width, int height );
    virtual void paintEvent( QPaintEvent * event );
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void tabletEvent ( QTabletEvent * event );
    void enterEvent( QEvent * );
    void leaveEvent( QEvent * );
    bool event( QEvent * event );

    TabletStateWidget * m_parent;
};

#endif
