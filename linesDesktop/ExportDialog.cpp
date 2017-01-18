#include "ExportDialog.h"
#include <QValidator>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>

ExportDialog::ExportDialog( QString & defaultPath, int defaultXRes, int defaultYRes, QWidget * parent ) : QDialog( parent )
{
	ui.setupUi(this);
	
	QValidator * validator = new QIntValidator( 1, 10000, this );
	ui.xResolutionEditBox->setValidator( validator );
	ui.yResolutionEditBox->setValidator( validator );
	
	QString xres;
	xres.setNum( defaultXRes );
	ui.xResolutionEditBox->setText( xres );
	QString yres;
	yres.setNum( defaultYRes );
	ui.yResolutionEditBox->setText( yres );

    ui.exportAlphaCheckBox->setChecked( false );
	
	ui.fileEditBox->setText( defaultPath );
}

ExportDialog::~ExportDialog()
{
}

void ExportDialog::GetResolution( QSize & res )
{
	res.setWidth( ui.xResolutionEditBox->text().toInt() );
	res.setHeight( ui.yResolutionEditBox->text().toInt() );
}

QString ExportDialog::GetFileName()
{
	return ui.fileEditBox->text();
}

bool ExportDialog::IsExportingAlpha()
{
    return ui.exportAlphaCheckBox->isChecked();
}

void ExportDialog::on_browseButton_clicked()
{
	QString currentPath = ui.fileEditBox->text();
	QFileInfo info( currentPath );
	if( info.isDir() )
	{
		currentPath += QDir::separator();
		currentPath += "untitled.swf";
	}
	
	QString newPath = QFileDialog::getSaveFileName( this, tr("Export to bitmap"),  currentPath, tr("PNG Files (*.png)") );
	if( !newPath.isEmpty() )
		ui.fileEditBox->setText( newPath );
}
