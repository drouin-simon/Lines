#ifndef __TabletStateWidget_h_
#define __TabletStateWidget_h_

#include <QWidget>
#include "ui_TabletStateWidget.h"

class drwDrawingWidget;

class TabletStateWidget : public QWidget
{
	
	Q_OBJECT
	
public:
	
	TabletStateWidget( QWidget * parent = 0 );
	~TabletStateWidget();

    void SetDrawingWiget( drwDrawingWidget * w );

    void LogMouseEvent( QMouseEvent * e );
    void LogTabletEvent( QTabletEvent * e );
    void LogEnterEvent();
    void LogLeaveEvent();
	
private slots:

    void on_clearLogButton_clicked();
    void on_moveEventsCheckBox_toggled(bool checked);
    void on_mouseTrackingCheckBox_toggled(bool checked);
    void on_muteMouseCheckBox_toggled(bool checked);
    void on_nativeEventCheckBox_toggled(bool checked);

private:
			 
	void UpdateUi( QTabletEvent * e );

    QString MouseEventTypeToString( QMouseEvent * e );
    QString TabletEventTypeToString( QTabletEvent * e );
    QString MouseEventButtonsToString( QMouseEvent * e );
    QString TabletEventButtonsToString( QTabletEvent * e );
    QString TabletEventToPointerType( QTabletEvent * e );
    QString TabletEventToDeviceType( QTabletEvent * e );

    drwDrawingWidget * m_drawingWidget;

	Ui::TabletStateWidget ui;
	
};

#endif
