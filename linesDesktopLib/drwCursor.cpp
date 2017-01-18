#include "drwCursor.h"
#include "drwLineTool.h"
#include "drwDrawingWidget.h"
#include <QPen>
#include <QPainter>

drwCursor::drwCursor( drwLineTool * lineTool, drwDrawingWidget * widget )
: m_lineTool( lineTool )
, m_drawingWidget( widget )
{
    m_position[0] = 0;
    m_position[1] = 0;
    m_colorName = "yellowgreen";
}

drwCursor::~drwCursor()
{
}

void drwCursor::SetPosition( int x, int y )
{
    m_position[0] = x;
    m_position[1] = y;
    emit Modified();
}

void drwCursor::Draw( QPainter & painter )
{
    double baseWidth = m_lineTool->GetBaseWidth();
    double pixPerUnit = m_drawingWidget->PixelsPerUnit();
    double dotPerPix = 1.0 / m_drawingWidget->devicePixelRatioF();
    double baseWidthPix = dotPerPix * pixPerUnit * baseWidth;

    QPen pen;
    pen.setColor( m_colorName );
    painter.setPen( pen );
    painter.setBrush( Qt::NoBrush );
    painter.drawEllipse( QPointF( m_position[0], m_position[1] ), baseWidthPix, baseWidthPix );
}
