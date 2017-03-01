#include "TabletStateTestingArea.h"
#include "TabletStateWidget.h"

TabletStateTestingArea::TabletStateTestingArea(QWidget *parent) : QOpenGLWidget(parent), m_logMoveEvents(true)
{
}

void TabletStateTestingArea::initializeGL()
{
}

void TabletStateTestingArea::resizeGL( int width, int height )
{
}

void TabletStateTestingArea::paintEvent( QPaintEvent * event )
{
}

void TabletStateTestingArea::mousePressEvent( QMouseEvent * e)
{
    m_parent->LogMouseEvent( e );
}

void TabletStateTestingArea::mouseReleaseEvent( QMouseEvent * e )
{
    m_parent->LogMouseEvent( e );
}

void TabletStateTestingArea::mouseMoveEvent( QMouseEvent * e)
{
    if( m_logMoveEvents )
        m_parent->LogMouseEvent( e );
}

void TabletStateTestingArea::tabletEvent ( QTabletEvent * e )
{
    if( e->type() != QEvent::TabletMove || m_logMoveEvents )
        m_parent->LogTabletEvent( e );
    e->ignore();
}

void TabletStateTestingArea::enterEvent( QEvent * )
{
    m_parent->LogEnterEvent();
}

void TabletStateTestingArea::leaveEvent( QEvent * )
{
    m_parent->LogLeaveEvent();
}

bool TabletStateTestingArea::event( QEvent * e )
{
    return QOpenGLWidget::event(e);
}
