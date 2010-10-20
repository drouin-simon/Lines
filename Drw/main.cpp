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
	
	// simtodo : remove this
	QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
	QString userName;
	QString machineName = QHostInfo::localHostName();
	if( pe.contains( QString("USER") ) )
		userName = pe.value( QString("USER") );
	else if( pe.contains( QString("USERNAME") ) )
		userName = pe.value( QString("USERNAME") );
	std::cout << "User name: " << userName.toAscii().data() << std::endl;
	std::cout << "Machine name: " << machineName.toAscii().data() << std::endl;
	
	QStringList allEnv = pe.toStringList();
	foreach( QString env, allEnv )
	{
		std::cout << env.toAscii().data() << std::endl;
	}
	
    return a.exec();
}

