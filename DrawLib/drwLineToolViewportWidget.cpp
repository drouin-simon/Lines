#include "drwLineToolViewportWidget.h"
#include "drwDrawingWidget.h"
#include <QPainter>
#include <QString>

drwLineToolViewportWidget::drwLineToolViewportWidget( drwDrawingWidget * w, int x, int y )
	: m_drawingWidget( w ), m_x( x ), m_y( y )
{
}

drwLineToolViewportWidget::~drwLineToolViewportWidget()
{
}

void drwLineToolViewportWidget::MousePress( int x, int y )
{
}

void drwLineToolViewportWidget::MouseRelease( int x, int y )
{
}

void drwLineToolViewportWidget::Draw()
{
	QPainter painter;
	painter.begin( m_drawingWidget );
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    
	// Draw the rectangle
	QRect buttonBounds( m_x, m_y, 100, 40 );
	painter.fillRect( buttonBounds, QColor( 255, 0, 0, 127 ) );
	
	// Draw text on button
	QString text = QString("Button");
    QFontMetrics metrics = QFontMetrics(m_drawingWidget->font());
    //int border = qMax( 4, metrics.leading() );
    QRect textBounds = metrics.boundingRect( buttonBounds, Qt::AlignCenter | Qt::TextWordWrap, text );
	painter.setPen( Qt::white );
	painter.drawText( textBounds, Qt::AlignCenter | Qt::TextWordWrap, text );
	
	painter.end();
}
