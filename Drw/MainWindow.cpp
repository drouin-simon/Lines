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
#include "drwLineToolViewportWidget.h"
#include "drwCursor.h"

#include <QtGui>

const QString MainWindow::m_appName( "Lines" );

MainWindow::MainWindow()
	: m_progressDialog(0)
{
    setWindowTitle( m_appName );

	CreateActions();

	// Create a Scene and a tool
	m_scene = new Scene(this);
	m_scene->SetNumberOfFrames( 30 );
	m_controler = new PlaybackControler(m_scene, this);
	m_localToolbox = new drwToolbox( m_scene, m_controler->GetEditionState(), this );
	m_commandDb = new drwCommandDatabase(this);
	m_networkManager = new drwNetworkManager();
	m_commandDispatcher = new drwCommandDispatcher( m_networkManager, m_commandDb, m_localToolbox, m_scene, this );
	m_networkManager->SetDispatcher( m_commandDispatcher );
	connect( m_networkManager, SIGNAL(StateChangedSignal()), this, SLOT(UpdateNetworkStatus()) );

	// Create main widget  (just a frame to put the viewing widget and the playback control widget)
	QWidget * mainWidget = new QWidget(this);
	setCentralWidget( mainWidget );
	QHBoxLayout * mainLayout = new QHBoxLayout(mainWidget);
	mainLayout->setContentsMargins( 0, 0, 0, 0 );
	mainLayout->setSpacing( 0 );

	QVBoxLayout * drawingAreaLayout = new QVBoxLayout(mainWidget);
	drawingAreaLayout->setContentsMargins( 0, 0, 0, 0 );
	mainLayout->addLayout( drawingAreaLayout );
	QVBoxLayout * rightPanelLayout = new QVBoxLayout( mainWidget );
	rightPanelLayout->setContentsMargins( 0, 10, 0, 10 );
	rightPanelLayout->setSpacing( 15 );
	mainLayout->addLayout( rightPanelLayout );
	
	// Create Drawing window
	m_glWidget = new drwDrawingWidget(mainWidget);
	m_glWidget->setMinimumSize( 400, 300 );
	m_glWidget->SetCurrentScene( m_scene );
	m_glWidget->SetObserver( m_localToolbox );
	m_glWidget->SetControler( m_controler );
	drawingAreaLayout->addWidget( m_glWidget );

    // Create special widget to go inside drawing widget
    drwLineTool * lineTool = dynamic_cast<drwLineTool*>(m_localToolbox->GetTool( 0 ));
    Q_ASSERT(lineTool);
    m_viewportWidget = new drwLineToolViewportWidget( m_glWidget, lineTool );
    m_glWidget->SetViewportWidget( m_viewportWidget );
    m_cursor = new drwCursor( lineTool, m_glWidget );
    m_glWidget->SetCursor( m_cursor );
	
	// Create playback control widget
	m_playbackControlerWidget = new PlaybackControlerWidget( m_glWidget->GetControler(), mainWidget );
	drawingAreaLayout->addWidget( m_playbackControlerWidget );

	// Alternative right panel
	m_toolOptionWidget = new PrimitiveToolOptionWidget( m_controler->GetEditionState(), lineTool, mainWidget );
	rightPanelLayout->addWidget( m_toolOptionWidget );
	m_displaySettingsWidget = new DisplaySettingsWidget( m_glWidget->GetDisplaySettings(), mainWidget );
	rightPanelLayout->addWidget( m_displaySettingsWidget );
	rightPanelLayout->addStretch();
	
	// Create tablet state dock
	m_dockTabletState = new QDockWidget(tr("Tablet State"));
	m_tabletStateWidget = new TabletStateWidget( m_dockTabletState );
	m_dockTabletState->setWidget(m_tabletStateWidget);
	m_viewMenu->addAction(m_dockTabletState->toggleViewAction());
	m_dockTabletState->setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable );

	// connect objects
	connect( m_localToolbox, SIGNAL(StartInteraction()), m_controler, SLOT(StartInteraction()) );
	connect( m_localToolbox, SIGNAL(EndInteraction()), m_controler, SLOT(EndInteraction()) );
	
	// Read Application settings
	readSettings();
	
	// Tell the qApp unique instance to send event to MainWindow::eventFilter before anyone else
	// so that we can grab global keyboard shortcuts.
	qApp->installEventFilter(this);
	
}

MainWindow::~MainWindow()
{
    delete m_viewportWidget;
    delete m_cursor;
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
	
	Reset();
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
	m_localToolbox->blockSignals( true );
	
    // Read commands from the file
	m_commandDb->Read( m_filename.toAscii() );

    // Dispatch commands
    m_commandDb->LockDb( true );
    for( int i = 0; i < m_commandDb->GetNumberOfCommands(); ++i )
    {
        drwCommand::s_ptr com = m_commandDb->GetCommand( i );
        m_commandDispatcher->IncomingDbCommand( com );
    }
    m_commandDb->LockDb( false );

	// Re-enable signal transmission
	m_localToolbox->blockSignals( false );
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
	// Get the user to select the server to connect to
	drwNetworkConnectDialog * dlg = new drwNetworkConnectDialog( this );
	int result = dlg->exec();
	QString name;
	QHostAddress address;
	if( result != QDialog::Accepted || !dlg->GetSelectedUserAndAddress( name, address ) )
	{
		delete dlg;
		UpdateNetworkStatus();
		return;
	}
	delete dlg;

	// Now we have a valid username and ip, try to connect
	Reset();
	m_scene->blockSignals( true );
	m_networkManager->Connect( name, address );

	// Create a timer to update a progress dialog
	QTimer timer;
	connect( &timer, SIGNAL(timeout()), this, SLOT(NetConnectProgress()) );
	timer.start( 200 );

	// Create progress dialog and start it
	m_progressDialog = new QProgressDialog( "Connecting", "Cancel", 0, 100 );
	m_progressDialog->setAttribute( Qt::WA_DeleteOnClose );
	m_progressDialog->exec();

	m_scene->blockSignals( false );
	
	// todo : If the operation didn't succeed, clear the scene again

	UpdateNetworkStatus();
}

void MainWindow::NetConnectProgress()
{
	drwNetworkManager::NetworkState state = m_networkManager->GetState();
	if( state == drwNetworkManager::WaitingForConnection )
	{
		m_progressDialog->setLabelText("Waiting for server...");
	}
	else if( state == drwNetworkManager::ReceivingScene )
	{
		// get progress and set it
		double percent = m_networkManager->GetPercentRead();
		m_progressDialog->setValue( percent );
		m_progressDialog->setLabelText("Receiving animation...");
	}
	else if( state == drwNetworkManager::Connected )
	{
		m_progressDialog->accept();
	}
	else
	{
		m_progressDialog->cancel();
	}
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
		m_toolOptionWidget->show();
		m_displaySettingsWidget->show();
	}
	else
	{
		showFullScreen();
		m_playbackControlerWidget->hide();
		m_toolOptionWidget->hide();
		m_displaySettingsWidget->hide();
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

	// Main window settings
	QRect mainWindowRect( 0, 0, 800, 600 );
	QPoint pos = settings.value( "MainWindow_pos", mainWindowRect.topLeft() ).toPoint();
	QSize size = settings.value( "MainWindow_size", mainWindowRect.size() ).toSize();
	resize( size );
	move( pos );
	
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
	
	// File dialog settings
	settings.setValue( "filedialogstartpath", m_fileDialogStartPath );
	settings.setValue( "ExportDefaultPath", m_exportDefaultPath );
	settings.setValue( "ExportResolution", m_exportRes );
	
}

void MainWindow::Reset()
{
	m_commandDispatcher->Reset();
	m_filename = "";
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
		else if( keyEvent->key() == Qt::Key_B )
		{
			m_localToolbox->ToggleBrushEraser();
			handled = true;
		}
        else if( keyEvent->key() == Qt::Key_R )
        {
            m_glWidget->ShowFullFrame( true );
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_Alt )
        {
            m_glWidget->ActivateViewportWidget( true );
        }
	} 
    else if( event->type() == QEvent::KeyRelease )
    {
        QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
        if( keyEvent->key() == Qt::Key_Alt )
        {
            m_glWidget->ActivateViewportWidget( false );
        }
    }
	if( !handled )
	{
		// standard event processing
		return QObject::eventFilter(obj, event);
	}
	return true;
}
