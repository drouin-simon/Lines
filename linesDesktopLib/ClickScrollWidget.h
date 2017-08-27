#ifndef __drwGradientWidget_h_
#define __drwGradientWidget_h_

#include <QWidget>

class QPoint;

class ClickScrollWidget : public QWidget
{
    Q_OBJECT

public:

    explicit ClickScrollWidget( QWidget *parent = 0);
    ~ClickScrollWidget();

	double sliderValue();
	void setSliderValue( double );

signals:

    void wheelClick( int n );

protected:

    void    mouseMoveEvent(QMouseEvent *event);
    void    mousePressEvent(QMouseEvent *event);
    void    mouseReleaseEvent(QMouseEvent *event);
    void    enterEvent(QEvent *event);
    void    leaveEvent(QEvent *event);
    void    paintEvent(QPaintEvent *event);

private:

    int  m_pixOffset;
    int  m_lastPos;
    int  m_startTouchPos;
    int  m_lastPosClick;
    bool m_leftButtonPressed;

    // Constants
    static const int m_clickInterval;
    static const int m_gratingOffset;
    static const int m_barInterval;
};

#endif
