#include <qapplication.h>
#include "MainWindow.h"

#include <QProcessEnvironment>
#include <QHostInfo>
#include <iostream>

int main( int argc, char** argv )
{
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support
    QApplication a( argc, argv );

    Q_INIT_RESOURCE(lineslibresources);

	// Setup main stylesheet
	QString styleSheet;
	styleSheet += "QWidget { background-color: rgb(61, 61, 61); color: white; }";
	styleSheet += "QPushButton { border: 1px solid rgb(120,120,120); border-radius: 5px; padding: 2px }";
    styleSheet += "QToolButton { border: 1px solid rgb(120,120,120); border-radius: 5px; padding: 2px }";
    styleSheet += "QToolButton:pressed { background-color: rgb(170,100,60); padding: 0 }";
    styleSheet += "QToolButton:checked { background-color: rgb(100, 100, 100); border: 2px solid rgb(120,120,120); border-radius: 5px; padding: 2px }";
	styleSheet += "QLineEdit { background-color: rgb(100,100,100); border: 1px solid rgb(120,120,120); border-radius: 5px; }";
	styleSheet += "QSpinBox { background-color: rgb(100,100,100); }";
	styleSheet += "QGroupBox { margin: 10px 0 0 0; padding: 0px 0 0 0; border: 1px solid rgb(120,120,120); border-radius: 5px; }";
	styleSheet += "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; margin: 0 }";

	
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    MainWindow mw;
	mw.setStyleSheet( styleSheet );
    mw.show();

    return a.exec();
}

