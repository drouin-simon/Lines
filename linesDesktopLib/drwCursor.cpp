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
    m_lastDrawnRect[0] = 0;
    m_lastDrawnRect[1] = 0;
    m_lastDrawnRect[2] = 0;
    m_lastDrawnRect[3] = 0;
}

drwCursor::~drwCursor()
{
}

void drwCursor::SetPosition( int x, int y )
{
    m_position[0] = x;
    m_position[1] = y;

    // Need to paint over last draw cursor
    m_drawingWidget->NeedRedrawOverlay( m_lastDrawnRect[0], m_lastDrawnRect[1], m_lastDrawnRect[2], m_lastDrawnRect[3]);

    // Need to paint around new cursor position
    int rect[4];
    double baseWidthPix = ComputeBaseWidthPix();
    ComputeCursorRect( baseWidthPix, m_position, rect );
    m_drawingWidget->NeedRedrawOverlay( rect[0], rect[1], rect[2], rect[3] );
}

void drwCursor::Draw( QPainter & painter )
{
    double baseWidthPix = ComputeBaseWidthPix();

    // Draw the cursor
    QPen pen;
    pen.setColor( m_colorName );
    painter.setPen( pen );
    painter.setBrush( Qt::NoBrush );
    painter.drawEllipse( QPointF( m_position[0], m_position[1] ), baseWidthPix, baseWidthPix );

    // Remember last place we drew the cursor
    ComputeCursorRect( baseWidthPix, m_position, m_lastDrawnRect );
}

double drwCursor::ComputeBaseWidthPix()
{
    double baseWidth = m_lineTool->GetBaseWidth();
    double pixPerUnit = m_drawingWidget->PixelsPerUnit();
    double dotPerPix = 1.0 / m_drawingWidget->devicePixelRatioF();
    double baseWidthPix = dotPerPix * pixPerUnit * baseWidth;
    return baseWidthPix;
}

void drwCursor::ComputeCursorRect( double baseWidth, int pos[2], int rect[4] )
{
    int radius = (int)ceil( baseWidth * 0.5 );
    rect[0] = pos[0] - radius;
    rect[1] = pos[1] - radius;
    rect[2] = 2 * radius;
    rect[3] = 2 * radius;
}
