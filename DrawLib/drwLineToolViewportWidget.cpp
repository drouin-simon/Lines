#include "drwLineToolViewportWidget.h"
#include "drwDrawingWidget.h"
#include "drwLineTool.h"
#include <QPainter>
#include <QString>

const int drwLineToolViewportWidget::m_width = 200;
const int drwLineToolViewportWidget::m_height = 150;
const int drwLineToolViewportWidget::m_spacing = 50;
const int drwLineToolViewportWidget::m_cursorWidth = 15;
const int drwLineToolViewportWidget::m_cursorHeight = 8;

drwLineToolViewportWidget::drwLineToolViewportWidget( drwDrawingWidget * w, drwLineTool * lt )
    : m_x(0), m_y(0), m_active(false),
    m_changingColor(false), m_changingSize(false), m_lastX(0), m_lastY(0),
    m_drawingWidget( w ), m_lineTool( lt )
{
}

drwLineToolViewportWidget::~drwLineToolViewportWidget()
{
}

void drwLineToolViewportWidget::Activate( int x, int y )
{
    m_x = x;
    m_y = y;
    m_active = true;
}

void drwLineToolViewportWidget::Deactivate()
{
    m_active = false;
}

void drwLineToolViewportWidget::MousePress( int x, int y )
{
}

void drwLineToolViewportWidget::MouseRelease( int x, int y )
{
}

void drwLineToolViewportWidget::MouseMove( int x, int y )
{

}

void drwLineToolViewportWidget::Draw()
{
    if( m_active )
    {
        QPainter painter;
        painter.begin( m_drawingWidget );
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        // Draw gradient
        int gradCorner[2];
        gradCorner[0] = m_x - m_width / 2;
        gradCorner[1] = m_y + m_spacing / 2;
        int widgetHeight = m_height / 2 - m_spacing / 2;
        QRect gradientRect( gradCorner[0], gradCorner[1], m_width, widgetHeight );
        QLinearGradient linearGrad( QPointF( gradCorner[0], 0 ), QPointF( gradCorner[0] + m_width, 0) );
        linearGrad.setColorAt( 0, Qt::black );
        linearGrad.setColorAt( 1, Qt::white );
        painter.setBrush( QBrush(linearGrad) );
        painter.drawRoundedRect( gradientRect, 5, 5 );

        // Draw slider
        // int cursorPosition = sliderValueToWidgetPos( m_sliderValue );
        float cursorPosition = 0.5;
        float cursorPixPosition = gradCorner[0] + m_width * cursorPosition;

        float top = (float)( gradCorner[1] + widgetHeight );
        float bottom = top + (float)m_cursorHeight;
        float left = cursorPixPosition - m_cursorWidth * .5;
        float right = cursorPixPosition + m_cursorWidth * .5;

        QPolygonF triangle;
        triangle.push_back( QPointF( left, bottom ) );
        triangle.push_back( QPointF( right, bottom ) );
        triangle.push_back( QPointF( cursorPixPosition, top ) );

        painter.drawPolygon( triangle );

        painter.end();
    }
}
