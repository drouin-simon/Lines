#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <math.h>

#include "drwGradientWidget.h"

drwGradientWidget::drwGradientWidget( QWidget *parent) :
		QWidget(parent),
        m_vertical( false ),
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

    setSliderValue( event->pos() );
    event->accept();
}

void drwGradientWidget::mousePressEvent( QMouseEvent * event )
{
	if( event->button() != Qt::LeftButton )
    {
        event->ignore();
        return;
    }

    setSliderValue( event->pos() );
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
        setSliderValue( event->pos() );
    }

	m_leftButtonPressed = false;
    event->accept();
}

void drwGradientWidget::paintEvent( QPaintEvent * event )
{
    Q_UNUSED(event)

    QPainter painter(this);
	painter.setRenderHint( QPainter::Antialiasing );

	// Draw gradient
    QPoint topLeft( 0, 0 );
    QSize s( width(), height() - m_cursorHeight - 2 );
    if( isVertical() )
        s = QSize( width() - m_cursorHeight - 2, height() );
    QRect gradientRect( topLeft, s );
    QPointF gradEnd( width(), 0.0 );
    if( isVertical() )
        gradEnd = QPointF( 0.0, height() );
    QLinearGradient linearGrad( QPointF( 0, 0 ), gradEnd );
	linearGrad.setColorAt( 0, Qt::black );
	linearGrad.setColorAt( 1, Qt::white );
	painter.setBrush( QBrush(linearGrad) );
	painter.drawRoundedRect( gradientRect, 5, 5 );

	// Draw slider
	int cursorPosition = sliderValueToWidgetPos( m_sliderValue );
	painter.setBrush( palette().windowText() );    
	QPolygonF triangle;
    if( isVertical() )
    {
        float bottom = (float)(rect().width() - 1);
        float top = (float)(rect().width() - m_cursorHeight - 1);
        float left = (float)(cursorPosition + m_cursorWidth * .5 );
        float right = (float)(cursorPosition - m_cursorWidth * .5);
        triangle.push_back( QPointF( left, bottom ) );
        triangle.push_back( QPointF( right, bottom ) );
        triangle.push_back( QPointF( float(cursorPosition), top ) );
    }
    else
    {
        float bottom = (float)(rect().height() - 1);
        float top = (float)(rect().height() - m_cursorHeight - 1);
        float left = (float)(cursorPosition - m_cursorWidth * .5 );
        float right = (float)(cursorPosition + m_cursorWidth * .5);
        triangle.push_back( QPointF( left, bottom ) );
        triangle.push_back( QPointF( right, bottom ) );
        triangle.push_back( QPointF( float(cursorPosition), top ) );
    }
	painter.drawPolygon( triangle );
}

void drwGradientWidget::setVertical( bool v )
{
    m_vertical = v;
    update();
}

bool drwGradientWidget::isVertical()
{
    return m_vertical;
}

void drwGradientWidget::setSliderValue( double val )
{
	m_sliderValue = val;
	emit sliderValueChanged( m_sliderValue );
	update();
}

void drwGradientWidget::setSliderValue( QPoint p )
{
    if( isVertical() )
        setSliderValue( (double)std::min( p.y(), height() ) / height() );
    setSliderValue( (double)std::min( p.x(), width() ) / width() );
}

int drwGradientWidget::sliderValueToWidgetPos( double sliderValue )
{
    if( isVertical() )
        return (int)round( sliderValue * height() );
	return (int)round( sliderValue * width() );
}
