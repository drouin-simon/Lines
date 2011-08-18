#include "drwLineToolViewportWidget.h"
#include "drwDrawingWidget.h"
#include "drwLineTool.h"
#include <QPainter>
#include <QString>

const int drwLineToolViewportWidget::m_width = 50;
const int drwLineToolViewportWidget::m_height = 200;
const int drwLineToolViewportWidget::m_spacing = 25;
const int drwLineToolViewportWidget::m_roundedCornerRadius = m_width / 2;
const int drwLineToolViewportWidget::m_scaleButtonRadius = 20;
const int drwLineToolViewportWidget::m_scaleSignRadius = 15;
const int drwLineToolViewportWidget::m_padding = 25;
const int drwLineToolViewportWidget::m_cursorWidth = 15;
const int drwLineToolViewportWidget::m_cursorHeight = 8;

static const QColor scaleWidgetColor( 125, 175, 200, 125 );
static const QColor scaleWidgetButtonColor( 130, 180, 210, 125 );
static const QColor boundingRectColor( 100, 150, 200, 125 );

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

    double intensity = m_lineTool->GetColor()[0];

    // compute scaling rect
    int scaleCorner[2];
    scaleCorner[0] = m_x - m_width / 2 - m_spacing - m_width;
    scaleCorner[1] = m_y - ( 1.0 - intensity ) * m_height;
    m_scaleRect.setRect( scaleCorner[0], scaleCorner[1], m_width, m_height );

    // compute gradient rect
    int gradCorner[2];
    gradCorner[0] = m_x - m_width / 2;
    gradCorner[1] = m_y - ( 1.0 - intensity ) * m_height;
    m_gradRect.setRect( gradCorner[0], gradCorner[1], m_width, m_height );

    // compute bounding rect
    int boundingCorner[2];
    boundingCorner[0] = scaleCorner[0] - m_padding;
    boundingCorner[1] = scaleCorner[1] - m_padding;
    int boundingSize[2];
    boundingSize[0] = 2 * m_width + m_spacing + 2 * m_padding;
    boundingSize[1] = m_height + 2 * m_padding;
    m_boundingRect.setRect( boundingCorner[0], boundingCorner[1], boundingSize[0], boundingSize[1] );
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
            ModifyColor( y );
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
            ModifyColor( y );
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
            ModifyColor( y );
            swallowEvent = true;
        }
        else if( m_changingSize )
        {
            double newBaseWidth = m_lineTool->GetBaseWidth();
            double diffY = y - m_lastY;
            newBaseWidth += diffY * -0.2;
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
        // Draw bounding rect
        painter.setBrush( QBrush( boundingRectColor ) );
        painter.drawRoundedRect( m_boundingRect, m_roundedCornerRadius, m_roundedCornerRadius );

        // Draw scaling rect
        //painter.setBrush( QBrush( Qt::gray ) );
        painter.setBrush( QBrush( scaleWidgetColor ) );
        painter.drawRoundedRect( m_scaleRect, m_roundedCornerRadius, m_roundedCornerRadius );

        // Draw + sign
        painter.setBrush( QBrush( scaleWidgetButtonColor ) );
        int plusX = m_scaleRect.x() + m_width / 2;
        int plusY = m_scaleRect.y() + m_width / 2;
        painter.drawEllipse( QPoint( plusX, plusY ), m_scaleButtonRadius, m_scaleButtonRadius );
        painter.setBrush( QBrush( Qt::white ) );
        painter.drawRect( plusX - m_scaleSignRadius, plusY - 1, 2 * m_scaleSignRadius, 3 );
        painter.drawRect( plusX - 1, plusY - m_scaleSignRadius, 3, 2 * m_scaleSignRadius );

        // Draw - sign
        painter.setBrush( QBrush( scaleWidgetButtonColor ) );
        int minusX = plusX;
        int minusY = m_scaleRect.bottom() - m_width / 2;
        painter.drawEllipse( QPoint( minusX, minusY), m_scaleButtonRadius, m_scaleButtonRadius );
        painter.setBrush( QBrush( Qt::white ) );
        painter.drawRect( minusX - m_scaleSignRadius, minusY - 1, 2 * m_scaleSignRadius, 3 );

        // Draw gradient
        QLinearGradient linearGrad( QPointF( 0, m_gradRect.y() + m_height ), QPointF( 0, m_gradRect.y() ) );
        linearGrad.setColorAt( 0, Qt::black );
        linearGrad.setColorAt( 1, Qt::white );
        painter.setBrush( QBrush(linearGrad) );
        painter.drawRoundedRect( m_gradRect, m_roundedCornerRadius, m_roundedCornerRadius );

        // Draw slider
        double sliderValue = m_lineTool->GetColor()[0];
        float cursorPosition = SliderValueToPos( sliderValue, m_gradRect );

        int sliderXMin = m_x - m_width / 2 + 3;
        int sliderYMin = cursorPosition - 1;
        QRect sliderRect( QPoint(sliderXMin, sliderYMin ), QSize( m_width - 6, 3 ) );
        painter.drawRect( sliderRect );

        /*float top = (float)( m_gradRect.y() + m_widgetHeight );
        float bottom = top + (float)m_cursorHeight;
        float left = cursorPosition - m_cursorWidth * .5;
        float right = cursorPosition + m_cursorWidth * .5;

        QPolygonF triangle;
        triangle.push_back( QPointF( left, bottom ) );
        triangle.push_back( QPointF( right, bottom ) );
        triangle.push_back( QPointF( cursorPosition, top ) );

        painter.drawPolygon( triangle );*/
    }
}

void drwLineToolViewportWidget::ModifyColor( int y )
{
    double value = PosToSliderValue( y, m_gradRect );
    if( value < 0.0 )
        value = 0.0;
    if( value > 1.0 )
        value = 1.0;
    double color = 1.0 - value;
    Vec4 newColor( color, color, color, 1.0 );
    m_lineTool->SetColor( newColor );
    m_drawingWidget->RequestRedraw();
}

double drwLineToolViewportWidget::PosToSliderValue( int y, QRect & r )
{
    double value = ( y - r.y() ) / double( r.height() );
    return value;
}

int drwLineToolViewportWidget::SliderValueToPos( double value, QRect & r )
{
    int pos = r.y() + int( ( 1.0 - value ) * r.height() );
    return pos;
}


