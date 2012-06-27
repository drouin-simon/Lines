#include "drwCursor.h"
#include "drwLineTool.h"
#include "drwDrawingWidget.h"
#include "Camera.h"

drwCursor::drwCursor( drwLineTool * lineTool, drwDrawingWidget * widget )
: m_lineTool( lineTool )
, m_drawingWidget( widget )
{
    m_position[0] = 0;
    m_position[1] = 0;
}

drwCursor::~drwCursor()
{
}

void drwCursor::SetPosition( int x, int y )
{
    m_position[0] = x;
    m_position[1] = y;
    double baseWidth = m_lineTool->GetBaseWidth();
    double pixPerUnit = m_drawingWidget->GetCamera()->PixelsPerUnit();
    double baseWidthPix = pixPerUnit * baseWidth;
    QRect modifiedRect( x - baseWidthPix, y - baseWidthPix, baseWidthPix, baseWidthPix );
    emit Modified( modifiedRect );
}

void drwCursor::Draw( QPainter & painter )
{
    double baseWidth = m_lineTool->GetBaseWidth();
    double pixPerUnit = m_drawingWidget->GetCamera()->PixelsPerUnit();
    double baseWidthPix = pixPerUnit * baseWidth;

    QPen pen( Qt::green, 1 );
    painter.setPen( pen );
    painter.setBrush( Qt::NoBrush );
    painter.drawEllipse( QPointF( m_position[0], m_position[1] ), baseWidthPix, baseWidthPix );
}
