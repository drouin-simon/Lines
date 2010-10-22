#include "MainWindow.h"
#include "drwDrawingWidget.h"
#include "Scene.h"
#include "drwLineTool.h"
#include "PlaybackControler.h"
#include "drwToolbox.h"
#include "drwCommandDatabase.h"
#include "PlaybackControlerWidget.h"
#include "PrimitiveToolOptionWidget.h"
#include "TabletStateWidget.h"
#include "DisplaySettingsWidget.h"
#include "ExportDialog.h"
#include "drwNetworkConnectDialog.h"
#include "drwEditionState.h"
#include "drwCommandDispatcher.h"
#include "drwNetworkManager.h"
#include "drwBitmapExporter.h"

#include <QtGui>

const QString MainWindow::m_appName( "Lines" );

MainWindow::MainWindow()
{
    setWindowTitle( m_appName );

	CreateActions();

	// Create a Scene and a tool
	m_scene = new Scene(this);
	m_scene->SetNumberOfFrames( 30 );
	m_controler = new PlaybackControler(m_scene, this);
	m_observer = new drwToolbox( 0, m_scene, m_controler->GetEditionState(), this );
	m_commandDb = new drwCommandDatabase(this);
    m_commandDispatcher = new drwCommandDispatcher( m_observer, m_scene, this );
    m_networkManager = new drwNetworkManager( m_commandDispatcher );

	// Create main widget  (just a frame to put the viewing widget and the playback control widget)
	QWidget * mainWidget = new QWidget(this);
	setCentralWidget( mainWidget );
	QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);
	mainLayout->setContentsMargins( 0, 0, 0, 0 );
	
    // Create Viewing window
	m_glWidget = new drwDrawingWidget(mainWidget);
	m_glWidget->setMinimumSize( 100, 100 );
	m_glWidget->SetCurrentScene( m_scene );
	m_glWidget->SetObserver( m_observer );
	m_glWidget->SetControler( m_controler );
	mainLayout->addWidget( m_glWidget );
	
	// Create playback control widget
	m_playbackControlerWidget = new PlaybackControlerWidget( m_glWidget->GetControler(), mainWidget );
	mainLayout->addWidget( m_playbackControlerWidget );
	
	// Create the toolbox
	m_dockToolsOptions = new QDockWidget(tr("Tool Options"));
	m_toolOptionWidget = new PrimitiveToolOptionWidget( m_controler->GetEditionState(), m_dockToolsOptions );
	m_dockToolsOptions->setWidget(m_toolOptionWidget);
	m_viewMenu->addAction(m_dockToolsOptions->toggleViewAction());
#ifdef Q_WS_X11
	m_dockToolsOptions->setAllowedAreas( Qt::RightDockWidgetArea );
	m_dockToolsOptions->setFeatures( QDockWidget::NoDockWidgetFeatures );
	addDockWidget( Qt::RightDockWidgetArea, m_dockToolsOptions );
#else
	m_dockToolsOptions->setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable );
	m_dockToolsOptions->setFloating(true);
	m_dockToolsOptions->show();
#endif
	
	// Create the DisplaySettings widget
	m_dockDisplayOptions = new QDockWidget(tr("Display options"));
	m_displaySettingsWidget = new DisplaySettingsWidget( m_glWidget->GetDisplaySettings() );
	m_dockDisplayOptions->setWidget(m_displaySettingsWidget);
	m_viewMenu->addAction(m_dockDisplayOptions->toggleViewAction());
#ifdef Q_WS_X11
	m_dockDisplayOptions->setAllowedAreas( Qt::RightDockWidgetArea );
	m_dockDisplayOptions->setFeatures( QDockWidget::NoDockWidgetFeatures );
	addDockWidget( Qt::RightDockWidgetArea, m_dockDisplayOptions );
#else
	m_dockDisplayOptions->setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable );
	m_dockDisplayOptions->setFloating(true);
	m_dockDisplayOptions->show();
#endif
	
	// Create tablet state dock
	m_dockTabletState = new QDockWidget(tr("Tablet State"));
	m_tabletStateWidget = new TabletStateWidget( m_dockTabletState );
	m_dockTabletState->setWidget(m_tabletStateWidget);
	m_viewMenu->addAction(m_dockTabletState->toggleViewAction());
	m_dockTabletState->setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable );
	//m_dockTabletState->setFloating(true);
	
	
	// connect objects
	connect( m_observer, SIGNAL( CommandExecuted(drwCommand::s_ptr) ), m_commandDb, SLOT( PushCommand( drwCommand::s_ptr ) ) );
	connect( m_observer, SIGNAL(StartInteraction()), m_controler, SLOT(StartInteraction()) );
	connect( m_observer, SIGNAL(EndInteraction()), m_controler, SLOT(EndInteraction()) );
	
	// Read Application settings
	readSettings();
	
	// Tell the qApp unique instance to send event to MainWindow::eventFilter before anyone else
	// so that we can grab global keyboard shortcuts.
	qApp->installEventFilter(this);
	
}

MainWindow::~MainWindow()
{
}

void MainWindow::CreateActions()
{
	// Creates a file menu
    QMenu * file = menuBar()->addMenu( "&File" );
	file->addAction( "New", this, SLOT( fileNew() ), Qt::CTRL + Qt::Key_N );
	file->addAction( "Open...", this, SLOT( fileOpen() ), Qt::CTRL + Qt::Key_O );
	file->addAction( "Save", this, SLOT( fileSave() ), Qt::CTRL + Qt::Key_S );
	file->addAction( "Save As...", this, SLOT( fileSaveAs() ), Qt::SHIFT + Qt::CTRL + Qt::Key_S );
	file->addAction( "Export...", this, SLOT( fileExport() ) );
    file->addAction( "&Exit", this, SLOT( close() ) );
	
	// Create the Edit menu
	m_editMenu = menuBar()->addMenu( "&Edit" );
	m_editMenu->addAction( "Set Number of Frames", this, SLOT( editSetNumberOfFrames() ), Qt::CTRL + Qt::Key_G );
	
	// Create the Network menu
	m_networkMenu = menuBar()->addMenu( "&Network" );
	m_netShareSessionMenuAction = m_networkMenu->addAction( "Share session", this, SLOT( NetShareSession() ), Qt::CTRL + Qt::Key_T );
	m_netConnectMenuItem = m_networkMenu->addAction( "Connect...", this, SLOT( NetConnect() ) );
	
	// Create the View menu
	m_viewMenu = menuBar()->addMenu( "&View" );
	m_viewMenu->addAction( "Fullscreen", this, SLOT( viewFullscreen() ), Qt::CTRL + Qt::Key_F );
	
    // Create a Help menu
    menuBar()->addSeparator();
    QMenu * help = menuBar()->addMenu( "&Help" );
    help->addSeparator();
    help->addAction( "&About", this, SLOT(about()) );
    help->addAction( "About&Qt", this, SLOT(aboutQt()));
}


// -------- File Menu implementation --------

void MainWindow::fileNew()
{
	if( !maybeSave() )
		return;
	
	m_commandDb->Clear();
	m_scene->Clear();
	m_filename = "";
}

void MainWindow::fileOpen()
{
	if( !maybeSave() )
		return;
	
	// Get the filename
	m_filename = QFileDialog::getOpenFileName(this, tr("Open Animation"),  m_fileDialogStartPath, tr("Animation Files (*.drw)"));
	if( m_filename.isEmpty() )
		return;
	
	// remember path
	m_fileDialogStartPath = m_filename;
	
	// block signal transmission before reading
	m_scene->blockSignals( true );
	m_observer->blockSignals( true );
	
	// Read
	drwCommandDispatcher * disp = new drwCommandDispatcher( m_observer, m_scene, this );
	connect( m_commandDb, SIGNAL( CommandRead(drwCommand::s_ptr) ), disp, SLOT( ExecuteCommand( drwCommand::s_ptr ) ) );
	m_commandDb->Read( m_filename.toAscii() );
	disp->deleteLater();
	
	// Re-enable signal transmission
	m_observer->blockSignals( false );
	m_scene->blockSignals( false );
	
	m_scene->MarkModified();
}

bool MainWindow::fileSave()
{
	if( m_filename.isEmpty() )
	{
		if( !GetSaveFilename() )
			return false;
	}
	
	m_commandDb->Write( m_filename.toAscii() );
	return true;
}

void MainWindow::fileSaveAs()
{
	if( !GetSaveFilename() )
		return;
	
	m_commandDb->Write( m_filename.toAscii() );
}

bool MainWindow::fileExport()
{
	ExportDialog dlg( m_exportDefaultPath, m_exportRes.width(), m_exportRes.height() );
	if( dlg.exec() == QDialog::Accepted )
	{
		// Get params from the dialog
		QString path = dlg.GetFileName();
		if( path.isEmpty() )
			return false;
		m_exportDefaultPath = path;
		dlg.GetResolution( m_exportRes );
		
		// Create the exporter and start it
		drwBitmapExporter * exporter = new drwBitmapExporter;
		exporter->SetFilename( path );
		exporter->SetScene( m_scene );
		exporter->SetGLWidget( m_glWidget );
		exporter->SetSize( m_exportRes );
		bool res = exporter->StartWriting();
		
		// Create the progress dialog, connect it to the exporter and start modal
		if( res )
		{
			QProgressDialog dialog;
			dialog.setLabelText("Exporting animation to bitmap");
			dialog.setRange( 0, m_scene->GetNumberOfFrames() - 1 );
			connect( exporter, SIGNAL(WritingFrame(int)), &dialog, SLOT(setValue(int)), Qt::QueuedConnection );
			dialog.exec();
		
			// wait for the exporter thread to terminate
			exporter->wait();
		
			return true;
		}
	}
	return false;
}

void MainWindow::editSetNumberOfFrames()
{
	int nbFrames = QInputDialog::getInt( this, "Set Number of Frames" , "Number of Frames", m_scene->GetNumberOfFrames(), 0 );
	m_scene->SetNumberOfFrames( nbFrames );
}

void MainWindow::NetShareSession()
{
    if( !m_networkManager->IsSharing() && !m_networkManager->IsConnected() )
	{
        m_networkManager->StartSharing();
		UpdateNetworkStatus();
	}
	else if( m_networkManager->IsSharing() )
	{
		m_networkManager->StopSharing();
		UpdateNetworkStatus();
	}
}

void MainWindow::NetConnect()
{
	drwNetworkConnectDialog * dlg = new drwNetworkConnectDialog( this );
	dlg->exec();
}

void MainWindow::UpdateNetworkStatus()
{
	if( m_networkManager->IsSharing() )
	{
		m_netShareSessionMenuAction->setText( tr("Stop Sharing" ) );
		m_netConnectMenuItem->setEnabled( false );
	}
	else 
	{
		m_netShareSessionMenuAction->setText( tr("Share Session") );
		m_netConnectMenuItem->setEnabled( true );
		if( m_networkManager->IsConnected() )
		{
			m_netConnectMenuItem->setText( tr("Disconnect") );
			m_netShareSessionMenuAction->setEnabled( false );
		}
		else 
		{
			m_netConnectMenuItem->setText( tr("Connect...") );
		}

	}

}

void MainWindow::viewFullscreen()
{
	if( isFullScreen() )
	{
		showNormal();
		m_playbackControlerWidget->show();
	}
	else
	{
		showFullScreen();
		m_playbackControlerWidget->hide();
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (maybeSave()) 
	{
		writeSettings();
		event->accept();
	} 
	else 
	{
		event->ignore();
	}
}

bool MainWindow::GetSaveFilename()
{
	m_filename = QFileDialog::getSaveFileName(this, tr("Save Animation"),  m_fileDialogStartPath + QDir::separator() + "untitled.drw", tr("Animation Files (*.drw)"));
	if( m_filename.isEmpty() )
		return false;
	m_fileDialogStartPath = m_filename;
	return true;
}

bool MainWindow::maybeSave()
{
	if( m_commandDb->IsModified() ) 
	{
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, m_appName, tr("The document has been modified.\nDo you want to save your changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if (ret == QMessageBox::Save)
			return fileSave();
		else if (ret == QMessageBox::Cancel)
			return false;
	}
	return true;
}

void MainWindow::readSettings()
{
	QSettings settings("SD", m_appName.toAscii() );
	
	// Compute widgets default sizes
	QSize hintToolsOptions = m_dockToolsOptions->sizeHint();
	QSize hintDisplayOptions = m_dockDisplayOptions->sizeHint();
	int sideBarWidth = std::max( hintToolsOptions.width(), hintDisplayOptions.width() );
	int toolsOptionsHeight = hintToolsOptions.height();
	QRect screenRect( 0, 0, 800, 600 ); // Assuming no screen is smaller than 800x600
	QDesktopWidget * desktop = QApplication::desktop();
	if( desktop )
		screenRect = desktop->availableGeometry( desktop->primaryScreen() );
	int drawAreaWidth = screenRect.width() - sideBarWidth;
	int drawAreaHeight = screenRect.height();
	
	QRect mainWindowRect( screenRect.topLeft(), QSize( drawAreaWidth, drawAreaHeight ) );
	QRect toolsOptionsRect( screenRect.x() + drawAreaWidth, screenRect.y(), sideBarWidth, toolsOptionsHeight );
	QRect displayOptionsRect( screenRect.x() + drawAreaWidth, screenRect.y() + toolsOptionsHeight, sideBarWidth, toolsOptionsHeight );
	
	// Main window settings
	QPoint pos = settings.value( "MainWindow_pos", mainWindowRect.topLeft() ).toPoint();
	QSize size = settings.value( "MainWindow_size", mainWindowRect.size() ).toSize();
	resize( size );
	move( pos );
	
	// Tools options dock settings
	pos = settings.value( "ToolsOptionsDock_pos", toolsOptionsRect.topLeft() ).toPoint();
	size = settings.value( "ToolsOptionsDock_size", toolsOptionsRect.size() ).toSize();
	bool visible = settings.value( "ToolsOptionsDock_visibility", true ).toBool();
	m_dockToolsOptions->setVisible( visible );
	m_dockToolsOptions->resize( size );
	m_dockToolsOptions->move( pos );
	
	// Display options dock settings
	pos = settings.value( "DisplayOptionsDock_pos", displayOptionsRect.topLeft() ).toPoint();
	size = settings.value( "DisplayOptionsDock_size", displayOptionsRect.size() ).toSize();
	visible = settings.value( "DisplayOptionsDock_visibility", true ).toBool();
	m_dockDisplayOptions->setVisible( visible );
	m_dockDisplayOptions->resize( size );
	m_dockDisplayOptions->move( pos );
	
	// Save path
	m_fileDialogStartPath = settings.value( "filedialogstartpath", QDir::homePath() ).toString();
	
	// Export settings
	m_exportDefaultPath = settings.value( "ExportDefaultPath", QDir::homePath() ).toString();
	m_exportRes = settings.value( "ExportResolution", QSize( 640, 360 ) ).toSize();
		
}


void MainWindow::writeSettings()
{
	// first get out of fullscreen mode
	if( isFullScreen() )
		viewFullscreen();
	
	QSettings settings( "SD", m_appName.toAscii() );
	
	// Main window settings
	settings.setValue( "MainWindow_pos", pos() );
	settings.setValue( "MainWindow_size", size() );
	
	// Tools options dock settings
	settings.setValue( "ToolsOptionsDock_pos", m_dockToolsOptions->pos() );
	settings.setValue( "ToolsOptionsDock_size", m_dockToolsOptions->size() );
	bool visible = m_dockToolsOptions->isVisible();
	settings.setValue( "ToolsOptionsDock_visibility", visible );
	
	// Display options dock settings
	settings.setValue( "DisplayOptionsDock_pos", m_dockDisplayOptions->pos() );
	settings.setValue( "DisplayOptionsDock_size", m_dockDisplayOptions->size() );
	visible = m_dockDisplayOptions->isVisible();
	settings.setValue( "DisplayOptionsDock_visibility", visible );
	
	// File dialog settings
	settings.setValue( "filedialogstartpath", m_fileDialogStartPath );
	settings.setValue( "ExportDefaultPath", m_exportDefaultPath );
	settings.setValue( "ExportResolution", m_exportRes );
	
}

void MainWindow::about()
{
    QMessageBox::about( this, m_appName, "Copyrights Simon Drouin 2008\n" );
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt( this, m_appName );
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	bool handled = false;
	if( event->type() == QEvent::KeyPress ) 
	{
		QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
		if( keyEvent->key() == Qt::Key_Right )
		{
			m_glWidget->GetControler()->NextFrame();
			handled = true;
		}
		else if( keyEvent->key() == Qt::Key_Left )
		{
			m_glWidget->GetControler()->PrevFrame();
			handled = true;
		}
		else if( keyEvent->key() == Qt::Key_Space )
		{
			m_glWidget->GetControler()->PlayPause();
			handled = true;
		}
		else if ( keyEvent->key() == Qt::Key_PageDown )
		{
			m_glWidget->GetControler()->GotoEnd();
			handled = true;
		}
		else if ( keyEvent->key() == Qt::Key_PageUp )
		{
			m_glWidget->GetControler()->GotoStart();
			handled = true;
		}
		else if( keyEvent->key() == Qt::Key_Comma )
		{
			int currentFrame = m_glWidget->GetControler()->GetCurrentFrame();
			m_scene->InsertFrame( currentFrame );
			m_glWidget->RequestRedraw();
		}
		else if( keyEvent->key() == Qt::Key_Period )
		{
			int currentFrame = m_glWidget->GetControler()->GetCurrentFrame();
			m_scene->InsertFrame( currentFrame + 1 );
			m_glWidget->GetControler()->NextFrame();
		}
		else if( keyEvent->key() == Qt::Key_B )
		{
			m_observer->ToggleBrushEraser();
		}
	} 
	if( !handled )
	{
		// standard event processing
		return QObject::eventFilter(obj, event);
	}
	return true;
}
