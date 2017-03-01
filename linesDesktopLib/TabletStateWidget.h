#ifndef __TabletStateWidget_h_
#define __TabletStateWidget_h_

#include <QWidget>
#include "ui_TabletStateWidget.h"

class TabletStateWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
	TabletStateWidget( QWidget * parent = 0 );
	~TabletStateWidget();

    void LogMouseEvent( QMouseEvent * e );
    void LogTabletEvent( QTabletEvent * e );
    void LogEnterEvent();
    void LogLeaveEvent();
	
private slots:

    void on_clearLogButton_clicked();

private:
			 
	void UpdateUi( QTabletEvent * e );

    QString MouseEventTypeToString( QMouseEvent * e );
    QString TabletEventTypeToString( QTabletEvent * e );
    QString MouseEventButtonsToString( QMouseEvent * e );
    QString TabletEventButtonsToString( QTabletEvent * e );

	Ui::TabletStateWidget ui;
	
};

#endif
