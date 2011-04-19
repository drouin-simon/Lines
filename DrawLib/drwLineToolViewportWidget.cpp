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
    m_drawingWidget( w ), m_lineTool( lt ), m_widgetHeight( 0 )
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

    // compute scaling rect
    m_widgetHeight = m_height / 2 - m_spacing / 2;
    int scaleCorner[2];
    scaleCorner[0] = m_x - m_width / 2;
    scaleCorner[1] = m_y - m_spacing / 2 - m_widgetHeight;
    m_scaleRect.setRect( scaleCorner[0], scaleCorner[1], m_width, m_widgetHeight );

    // compute gradient rect
    int gradCorner[2];
    gradCorner[0] = m_x - m_width / 2;
    gradCorner[1] = m_y + m_spacing / 2;
    m_gradRect.setRect( gradCorner[0], gradCorner[1], m_width, m_widgetHeight );
}

void drwLineToolViewportWidget::Deactivate()
{
    m_active = false;
    m_changingColor = false;
    m_changingSize = false;
}

bool drwLineToolViewportWidget::MousePress( int x, int y )
{
    bool swallowEvent = false;
    if( m_active )
    {
        if( m_gradRect.contains( x, y ) )
        {
            ModifyColor( x );
            m_changingColor = true;
            swallowEvent = true;
        }
        else if( m_scaleRect.contains( x, y ) )
        {
            m_changingSize = true;
            m_lastX = x;
            m_lastY = y;
            swallowEvent = true;
        }
    }
    return swallowEvent;
}

bool drwLineToolViewportWidget::MouseRelease( int x, int y )
{
    bool swallowEvent = false;
    if( m_active )
    {
        if( m_changingColor )
        {
            ModifyColor( x );
            m_changingColor = false;
            swallowEvent = true;
        }
        else if( m_changingSize )
        {
            m_changingSize = false;
            swallowEvent = true;
        }
    }
    return swallowEvent;
}

bool drwLineToolViewportWidget::MouseMove( int x, int y )
{
    bool swallowEvent = false;
    if( m_active )
    {
        if( m_changingColor )
        {
            ModifyColor( x );
            swallowEvent = true;
        }
        else if( m_changingSize )
        {
            double newBaseWidth = m_lineTool->GetBaseWidth();
            double diffX = x - m_lastX;
            newBaseWidth += diffX * -0.2;
            m_lineTool->SetBaseWidth( newBaseWidth );
            m_lastX = x;
            m_lastY = y;
            swallowEvent = true;
            m_drawingWidget->RequestRedraw();
        }
    }
    return swallowEvent;
}

void drwLineToolViewportWidget::Draw( QPainter & painter )
{
    if( m_active )
    {
        // Draw scaling rect
        painter.setBrush( QBrush( Qt::gray ) );
        painter.drawRoundedRect( m_scaleRect, 5, 5 );

        // Draw gradient
        QLinearGradient linearGrad( QPointF( m_gradRect.x(), 0 ), QPointF( m_gradRect.x() + m_width, 0) );
        linearGrad.setColorAt( 0, Qt::black );
        linearGrad.setColorAt( 1, Qt::white );
        painter.setBrush( QBrush(linearGrad) );
        painter.drawRoundedRect( m_gradRect, 5, 5 );

        // Draw slider
        double sliderValue = m_lineTool->GetColor()[0];
        float cursorPosition = SliderValueToPos( sliderValue, m_gradRect );

        float top = (float)( m_gradRect.y() + m_widgetHeight );
        float bottom = top + (float)m_cursorHeight;
        float left = cursorPosition - m_cursorWidth * .5;
        float right = cursorPosition + m_cursorWidth * .5;

        QPolygonF triangle;
        triangle.push_back( QPointF( left, bottom ) );
        triangle.push_back( QPointF( right, bottom ) );
        triangle.push_back( QPointF( cursorPosition, top ) );

        painter.drawPolygon( triangle );
    }
}

void drwLineToolViewportWidget::ModifyColor( int x )
{
    double value = PosToSliderValue( x, m_gradRect );
    if( value < 0.0 )
        value = 0.0;
    if( value > 1.0 )
        value = 1.0;
    Vec4 newColor( value, value, value, 1.0 );
    m_lineTool->SetColor( newColor );
    m_drawingWidget->RequestRedraw();
}

double drwLineToolViewportWidget::PosToSliderValue( int x, QRect & r )
{
    double value = ( x - r.x() ) / double( r.width() );
    return value;
}

int drwLineToolViewportWidget::SliderValueToPos( double value, QRect & r )
{
    int pos = r.x() + int(value * r.width());
    return pos;
}


