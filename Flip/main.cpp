#include <qapplication.h>
#include "mainwindow.h"

int main( int argc, char** argv )
{
    // Create GUI app and main window
    QApplication a( argc, argv );
    MainWindow * mw = new MainWindow( 0 );
	mw->setAttribute(Qt::WA_DeleteOnClose);
    mw->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );

    // Start main loop
    return a.exec();
}

