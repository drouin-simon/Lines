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
	
	void tabletEvent ( QTabletEvent * event );
	
private:
			 
	void UpdateUi( QTabletEvent * e );
	
	Ui::TabletStateWidget ui;
	
};

#endif