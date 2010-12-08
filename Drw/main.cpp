#include <qapplication.h>
#include "MainWindow.h"

#include <QProcessEnvironment>
#include <QHostInfo>
#include <iostream>

int main( int argc, char** argv )
{
    QApplication a( argc, argv );
    MainWindow mw;
    mw.show();
	
    return a.exec();
}

