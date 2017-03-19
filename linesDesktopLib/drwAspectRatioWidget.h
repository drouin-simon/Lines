#ifndef __drwAspectRatioWidget_h_
#define __drwAspectRatioWidget_h_

#include <QWidget>

class QBoxLayout;

class drwAspectRatioWidget : public QWidget
{

    Q_OBJECT

public:

    explicit drwAspectRatioWidget( QWidget * parent = 0 );
    void resizeEvent( QResizeEvent * event );
    void setClientWidget( QWidget * w );
    void setAspectRatioEnabled( bool e );

private:

    void UpdateStretch( int w, int h );

    QBoxLayout * m_layout;
    QWidget * m_widget;
    bool m_aspectRatioEnabled;
};

#endif
