#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <math.h>

#include "drwGradientWidget.h"

drwGradientWidget::drwGradientWidget(const QColor &color, QWidget *parent) :
		QWidget(parent),
		m_cursorWidth( 10 ),
		m_cursorHeight( 5 ),
		m_sliderValue( .5 ),
		m_leftButtonPressed(false)
{
}

drwGradientWidget::~drwGradientWidget()
{
}

void drwGradientWidget::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    setCursor(Qt::CrossCursor);
}

void drwGradientWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    unsetCursor();
}

void drwGradientWidget::mouseMoveEvent( QMouseEvent * event )
{
	if (!m_leftButtonPressed)
    {
        event->ignore();
        return;
    }

	int cursorWindowPos = 0;
	if( event->pos().x() >= rect().width() )
		cursorWindowPos = rect().width() - 1;
	else if( event->pos().x() < 0 )
		cursorWindowPos = 0;
	else
		cursorWindowPos = event->pos().x();

	setSliderValue( cursorWindowPos );
    event->accept();
}

void drwGradientWidget::mousePressEvent( QMouseEvent * event )
{
	if( event->button() != Qt::LeftButton )
    {
        event->ignore();
        return;
    }

	setSliderValue( event->pos().x() );
	m_leftButtonPressed = true;
    event->accept();
}

void drwGradientWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }

    // On teste que la souris est bien dans le widget
	if( rect().contains( event->pos() ) )
    {
		setSliderValue( event->pos().x() );
    }

	m_leftButtonPressed = false;
    event->accept();
}

void drwGradientWidget::paintEvent( QPaintEvent * event )
{
    Q_UNUSED(event)

    QPainter painter(this);
	painter.setRenderHint( QPainter::HighQualityAntialiasing );

	// Draw gradient
	QRect gradientRect( 0, 0, width(), height() - m_cursorHeight - 2 );
	QLinearGradient linearGrad( QPointF( 0, 0 ), QPointF( width(), 0) );
	linearGrad.setColorAt( 0, Qt::black );
	linearGrad.setColorAt( 1, Qt::white );
	painter.setBrush( QBrush(linearGrad) );
	painter.drawRoundedRect( gradientRect, 5, 5 );

	// Draw slider
	int cursorPosition = sliderValueToWidgetPos( m_sliderValue );
	painter.setBrush( palette().windowText() );
	float bottom = (float)(rect().height() - 1);
	float top = (float)(rect().height() - m_cursorHeight - 1);
	float left = (float)(cursorPosition - m_cursorWidth * .5 );
	float right = (float)(cursorPosition + m_cursorWidth * .5);
	QPolygonF triangle;
	triangle.push_back( QPointF( left, bottom ) );
	triangle.push_back( QPointF( right, bottom ) );
	triangle.push_back( QPointF( float(cursorPosition), top ) );
	painter.drawPolygon( triangle );
}

void drwGradientWidget::setSliderValue( double val )
{
	m_sliderValue = val;
	emit sliderValueChanged( m_sliderValue );
	update();
}

void drwGradientWidget::setSliderValue( int val )
{
	setSliderValue( widgetPosToSliderValue( val ) );
}

double drwGradientWidget::widgetPosToSliderValue( int widgetPos )
{
	double sliderValue = (double)widgetPos / width();
	return sliderValue;
}

int drwGradientWidget::sliderValueToWidgetPos( double sliderValue )
{
	return (int)round( sliderValue * width() );
}
