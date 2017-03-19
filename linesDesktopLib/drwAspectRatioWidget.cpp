#include "drwAspectRatioWidget.h"
#include <QResizeEvent>
#include <QBoxLayout>
#include <math.h>

drwAspectRatioWidget::drwAspectRatioWidget( QWidget * parent ) : QWidget(parent)
{
    m_aspectRatioEnabled = true;
    m_layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    m_layout->setContentsMargins( 0, 0, 0, 0 );

    // add spacer, then your widget, then spacer
    m_layout->addItem( new QSpacerItem(0, 0) );
    m_widget = new QWidget( this );
    m_layout->addWidget( m_widget );
    m_layout->addItem( new QSpacerItem(0, 0) );
}

void drwAspectRatioWidget::resizeEvent( QResizeEvent * event )
{
    int w = event->size().width();
    int h = event->size().height();
    UpdateStretch( w, h );
}

void drwAspectRatioWidget::setClientWidget( QWidget * w )
{
    m_layout->replaceWidget( m_widget, w );
    m_widget = w;
}

void drwAspectRatioWidget::setAspectRatioEnabled( bool e )
{
    m_aspectRatioEnabled = e;
    UpdateStretch( this->width(), this->height() );
}

void drwAspectRatioWidget::UpdateStretch( int w, int h )
{
    if( m_aspectRatioEnabled )
    {
        float widgetAspect = (float)w / h;
        float childAspect = 16.0 / 9.0;
        int widgetStretch, outerStretch;

        if( widgetAspect > childAspect ) // too wide
        {
            m_layout->setDirection(QBoxLayout::LeftToRight);
            widgetStretch = (int)round( h * childAspect);
            outerStretch = (int)round( ( w - widgetStretch ) * 0.5 );
        }
        else // too tall
        {
            m_layout->setDirection(QBoxLayout::TopToBottom);
            widgetStretch = (int)round( w / childAspect );
            outerStretch = (int)round( ( h - widgetStretch ) * 0.5 );
        }

        m_layout->setStretch(0, outerStretch);
        m_layout->setStretch(1, widgetStretch);
        m_layout->setStretch(2, outerStretch);
    }
    else
    {
        m_layout->setStretch(0, 0);
        m_layout->setStretch(1, 1);
        m_layout->setStretch(2, 0);
    }
}
