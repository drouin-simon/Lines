#include "MainWindow.h"
#include "drwDrawingWidget.h"
#include "Scene.h"
#include "ImageFlippingTool.h"

#include <QApplication>
#include <QMenu>
#include <QMenubar>
#include <QStatusbar>
#include <QMessagebox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QDockWidget>
#include <QVBoxLayout>


const QString MainWindow::m_appName( "Draw" );


MainWindow::MainWindow()
{
    setWindowTitle( m_appName );

    //statusBar()->setHidden( TRUE );

    // Creates a file menu
    QMenu * file = menuBar()->addMenu( "&File" );
    file->addAction( "Settings...", this, SLOT( fileSettings() ) );
    file->addAction( "&Exit", qApp, SLOT( closeAllWindows() ) );

    // Create a Help menu
    menuBar()->addSeparator();
    QMenu * help = menuBar()->addMenu( "&Help" );
    help->addSeparator();
    help->addAction( "&About", this, SLOT(about()) );
    help->addAction( "About&Qt", this, SLOT(aboutQt()));

	// Create a Scene and a tool
	m_scene = new Scene;
	m_flippingTool = new ImageFlippingTool( m_scene );

    // Create Viewing window
    m_glWidget = new drwDrawingWidget( this );
	m_glWidget->SetCurrentScene( m_scene );
	m_glWidget->SetObserver( m_flippingTool );
	setCentralWidget( m_glWidget );
	m_glWidget->show();

	// Create the toolbox
	//QDockWidget * dock = new QDockWidget(tr("Toolbox"), this);
	//addDockWidget(Qt::LeftDockWidgetArea, dock);
	//dock->show();

    this->showMaximized();
	//this->show();
    statusBar()->showMessage( "Ready", 2000 );

}


MainWindow::~MainWindow()
{
	delete m_scene;
	delete m_flippingTool;
}


void MainWindow::about()
{
    QMessageBox::about( this, m_appName, "Copyrights Simon Drouin 2007\n" );
}


void MainWindow::aboutQt()
{
    QMessageBox::aboutQt( this, m_appName );
}


void MainWindow::fileSettings()
{
    //QWidget * res = Application::GetSceneManager()->CreateSettingsDialog( 0 );
    //res->show();
}
