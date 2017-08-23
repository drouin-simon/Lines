#ifndef __drwGradientWidget_h_
#define __drwGradientWidget_h_

#include <QWidget>

class QPoint;

class drwGradientWidget : public QWidget
{
    Q_OBJECT

public:

    explicit drwGradientWidget( QWidget *parent = 0);
	~drwGradientWidget();

    void setVertical( bool v );
    bool isVertical();

	double sliderValue();
	void setSliderValue( double );

signals:

	void sliderValueChanged( double value );

protected:

    void    mouseMoveEvent(QMouseEvent *event);
    void    mousePressEvent(QMouseEvent *event);
    void    mouseReleaseEvent(QMouseEvent *event);
    void    enterEvent(QEvent *event);
    void    leaveEvent(QEvent *event);
    void    paintEvent(QPaintEvent *event);

private:

    void   setSliderValue( QPoint p );
	int    sliderValueToWidgetPos( double );

    bool    m_vertical;
	int     m_cursorWidth;
	int		m_cursorHeight;
	double  m_sliderValue;

	bool    m_leftButtonPressed;
};

#endif
