#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <cmath>

#include "ClickScrollWidget.h"

const int ClickScrollWidget::m_clickInterval = 40;
const int ClickScrollWidget::m_gratingOffset = 5;
const int ClickScrollWidget::m_barInterval = 10;

ClickScrollWidget::ClickScrollWidget( QWidget *parent) :
		QWidget(parent),
        m_pixOffset( 0 ),
        m_startTouchPos( 0 ),
        m_lastPosClick( 0 ),
		m_leftButtonPressed(false)
{
}

ClickScrollWidget::~ClickScrollWidget()
{
}

void ClickScrollWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    setCursor(Qt::CrossCursor);
}

void ClickScrollWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    unsetCursor();
}

void ClickScrollWidget::mouseMoveEvent( QMouseEvent * event )
{
	if (!m_leftButtonPressed)
    {
        event->ignore();
        return;
    }

    int currentPos = event->pos().y();
    m_pixOffset += currentPos - m_lastPos;
    m_lastPos = currentPos;

    int curPosClick = ( currentPos - m_startTouchPos ) / m_clickInterval;
    if( curPosClick != m_lastPosClick )
    {
        emit wheelClick( curPosClick - m_lastPosClick );
        m_lastPosClick = curPosClick;
    }

    event->accept();
    update();
}

void ClickScrollWidget::mousePressEvent( QMouseEvent * event )
{
	if( event->button() != Qt::LeftButton )
    {
        event->ignore();
        return;
    }

    m_lastPos = event->pos().y();
    m_lastPosClick = 0;
    m_startTouchPos = m_lastPos;
	m_leftButtonPressed = true;
    event->accept();
}

void ClickScrollWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }

	m_leftButtonPressed = false;
    event->accept();
}

void ClickScrollWidget::paintEvent( QPaintEvent * event )
{
    Q_UNUSED(event)

    QPainter painter(this);
	painter.setRenderHint( QPainter::Antialiasing );

    // Draw bounding box
    painter.setPen( Qt::white );
    painter.drawRoundedRect( 0, 0, width(), height(), 5.0, 5.0 );

    // Draw grating
    QVector<QPointF> gratings;
    int startY = m_pixOffset % m_barInterval;
    if( startY < 0 )
        startY += m_barInterval;
    while( startY < height() - 1 )
    {
        gratings.push_back( QPoint( m_gratingOffset, startY ) );
        gratings.push_back( QPoint( width() - m_gratingOffset, startY ) );
        startY += m_barInterval;
    }
    painter.drawLines( gratings );
}

