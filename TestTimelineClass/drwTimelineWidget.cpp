#include "drwTimelineWidget.h"
#include "drwScene.h"
#include <QtGui>

drwTimelineWidget::drwTimelineWidget( QWidget * parent ) 
: QWidget( parent )
, m_scene( 0 )
, m_layerHeight( 30 )
, m_frameLength( 20 )
, currentMouseX( 0 )
, currentMouseY( 0 )
, lastMouseX( 0 )
, lastMouseY( 0 )
, mouseMoving( false )
, isScaling( false )
, isPaning( false )
, isMoving( false )
{
	setAcceptDrops(true);
	setMouseTracking(true);
}

void drwTimelineWidget::SetScene( drwScene * scene )
{
	m_scene = scene;
	update();
}

static const QPoint hourHand[3] = {
	QPoint(7, 8),
	QPoint(-7, 8),
	QPoint(0, -40)
};
static const QPoint minuteHand[3] = {
	QPoint(7, 8),
	QPoint(-7, 8),
	QPoint(0, -70)
};

void drwTimelineWidget::paintEvent(QPaintEvent *)
{
	QColor hourColor(127, 0, 127);
	QColor minuteColor(0, 127, 127, 191);

	int side = qMin(width(), height());

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(minuteColor);
	painter.setBrush(hourColor);
	
	int nbLayers = m_scene->GetNbLayers();
	for( int i = 0; i < nbLayers; ++i )
	{
		int layerStart = 0;
		int layerLength = 0;
		m_scene->GetLayerBounds( i, layerStart, layerLength );
		layerStart *= m_frameLength;
		int layerEnd = ( layerStart + layerLength ) * m_frameLength;
		painter.drawRect( layerStart, 0, layerEnd, m_layerHeight );
		painter.translate( qreal(0), (qreal)m_layerHeight );
	}
}


void drwTimelineWidget::mousePressEvent( QMouseEvent * e )
{
	if ( e->button() == Qt::LeftButton )
	{
		if( HitCell( e->x(), e->y() ) )
		{
			isMoving = true;
			lastMouseX = e->x();
			lastMouseY = e->y();
		}
	}
}


void drwTimelineWidget::mouseReleaseEvent( QMouseEvent * e )
{
	if ( e->button() == Qt::LeftButton )
	{
		isMoving = false;
	}
}


void drwTimelineWidget::mouseMoveEvent( QMouseEvent * e )
{
	lastMouseX = currentMouseX;
	lastMouseY = currentMouseY;
	currentMouseX = e->x();
	currentMouseY = e->y();

	if( isScaling )
	{
	}
	else if( isPaning )
	{
	}
	else
	{
	}
}
