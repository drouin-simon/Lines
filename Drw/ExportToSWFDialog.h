#ifndef __ExportToSWFDialog_h_
#define __ExportToSWFDialog_h_

#include <QDialog>
#include "ui_ExportToSWFDialog.h"

class ExportToSWFDialog : public QDialog
{
	
	Q_OBJECT
	
public:
	
	ExportToSWFDialog( QString & path, int width, int height, QWidget * parent = 0 );
	~ExportToSWFDialog();
	
	void GetResolution( QSize & res );
	QString GetFileName();
	
private slots:
	
	void on_browseButton_clicked();
	
private:
	
	Ui::ExportToSWFDialog ui;
	
};

#endif