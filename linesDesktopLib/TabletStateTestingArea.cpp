#include "TabletStateTestingArea.h"
#include "TabletStateWidget.h"

TabletStateTestingArea::TabletStateTestingArea(QWidget *parent) : QOpenGLWidget(parent)
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
    m_parent->LogMouseEvent( e );
}

void TabletStateTestingArea::tabletEvent ( QTabletEvent * e )
{
    m_parent->LogTabletEvent( e );
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
