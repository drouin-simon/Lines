#include <QApplication>
#include "MainWindow.h"

int main( int argc, char** argv )
{
    QApplication a( argc, argv );

    // Setup main stylesheet
    QString styleSheet;
    styleSheet += "QWidget { background-color: rgb(61, 61, 61); color: white; }";
    styleSheet += "QPushButton { border: 1px solid rgb(120,120,120); border-radius: 5px; padding: 2px }";
    styleSheet += "QLineEdit { background-color: rgb(100,100,100); border: 1px solid rgb(120,120,120); border-radius: 5px; }";
    styleSheet += "QSpinBox { background-color: rgb(100,100,100); }";
    styleSheet += "QGroupBox { margin: 10px 0 0 0; padding: 0px 0 0 0; border: 1px solid rgb(120,120,120); border-radius: 5px; }";
    styleSheet += "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; margin: 0 }";

    MainWindow mw;
    mw.setStyleSheet( styleSheet );
    mw.show();

    return a.exec();
}

