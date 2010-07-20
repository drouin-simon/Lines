#include <qapplication.h>
#include "MainWindow.h"
#include "mingpp.h"

int main( int argc, char** argv )
{
    QApplication a( argc, argv );
    MainWindow mw;
    mw.show();
    return a.exec();
}

