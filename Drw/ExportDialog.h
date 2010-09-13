#ifndef __ExportDialog_h_
#define __ExportDialog_h_

#include <QDialog>
#include "ui_ExportDialog.h"

class ExportDialog : public QDialog
{
	
	Q_OBJECT
	
public:
	
	ExportDialog( QString & path, int width, int height, QWidget * parent = 0 );
	~ExportDialog();
	
	void GetResolution( QSize & res );
	QString GetFileName();
	
private slots:
	
	void on_browseButton_clicked();
	
private:
	
	Ui::ExportDialog ui;
	
};

#endif