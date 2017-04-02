#include "MainWindow.h"
#include "drwDrawingWidget.h"
#include "drwAspectRatioWidget.h"
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
#include "drwCommandDispatcher.h"
#include "drwNetworkManager.h"
#include "drwBitmapExporter.h"
#include "drwLineToolViewportWidget.h"
#include "drwCursor.h"
#include "drwGlobalLineParams.h"
#include "Vec4.h"
#include <QtWidgets>
#include "drwsimplifiedtoolbar.h"
#include "LinesApp.h"

const QString MainWindow::m_appName( "Lines" );

MainWindow::MainWindow()
	: m_progressDialog(0)
{
    setWindowTitle( m_appName );
    m_whiteOnBlack = true;
    m_eraseToggled = false;
    m_simplifiedGui = true;
    m_guiHidden = false;

	CreateActions();

	// Create a Scene and a tool
	m_scene = new Scene(this);
    m_controler = new PlaybackControler( m_scene );
    m_localToolbox = new drwToolbox( m_scene, m_controler );
    m_controler->SetToolbox( m_localToolbox );
	m_commandDb = new drwCommandDatabase(this);
	m_networkManager = new drwNetworkManager();
	m_commandDispatcher = new drwCommandDispatcher( m_networkManager, m_commandDb, m_localToolbox, m_scene, this );
	m_networkManager->SetDispatcher( m_commandDispatcher );
    connect( m_networkManager, SIGNAL(StateChangedSignal()), this, SLOT(NetStateChanged()) );
    m_scene->SetNumberOfFrames( 24 ); // do this after everything else is initialized to make sure we generate a command for the db.
    m_globalLineParams = new drwGlobalLineParams( this );
    m_globalLineParamsDock = 0;

    m_linesApp = new LinesApp( m_localToolbox );

	// Create main widget  (just a frame to put the viewing widget and the playback control widget)
    m_mainWidget = new QWidget(this);
    setCentralWidget( m_mainWidget );

    QHBoxLayout * mainLayout = new QHBoxLayout( m_mainWidget );
    mainLayout->setContentsMargins( 0, 0, 0, 0 );
    mainLayout->setSpacing( 0 );

    // Left panel widget (simplified toolbar)
    m_simplifiedToolbar = new drwSimplifiedToolbar( m_mainWidget );
    m_simplifiedToolbar->SetApp( m_linesApp );
    mainLayout->addWidget( m_simplifiedToolbar );
    m_simplifiedToolbar->setHidden( !m_simplifiedGui );

    // Drawing area layout (drawing window + timeline)
    QVBoxLayout * drawingAreaLayout = new QVBoxLayout();
	drawingAreaLayout->setContentsMargins( 0, 0, 0, 0 );
    mainLayout->addLayout( drawingAreaLayout );

    // Right panel widget
    m_rightPanelDock = new QDockWidget( tr("Right Panel"), this );

    QWidget * defaultTitleBar = m_rightPanelDock->titleBarWidget();
    QWidget * emptyTitleBar = new QWidget();
    m_rightPanelDock->setTitleBarWidget(emptyTitleBar);
    delete defaultTitleBar;

    m_rightPanelDock->setFeatures( QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable );
    m_rightPanelDock->setAllowedAreas( Qt::RightDockWidgetArea );
    addDockWidget( Qt::RightDockWidgetArea, m_rightPanelDock );

    m_rightPanelWidget = new QWidget( m_rightPanelDock );
    QVBoxLayout * rightPanelLayout = new QVBoxLayout( m_rightPanelWidget );
    rightPanelLayout->setContentsMargins( 0, 10, 0, 10 );
    rightPanelLayout->setSpacing( 15 );
    m_rightPanelDock->setWidget( m_rightPanelWidget );
    m_rightPanelDock->setHidden( m_simplifiedGui ); // by default, this is hidden

    // Set default OpenGL surface format that will be applied to all openGL windows
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    //format.setVersion(3, 2);
    //format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
    QSurfaceFormat::setDefaultFormat(format);

    // Create Drawing window container that maintains a 16:9 aspect ratio
    m_drawingWidgetContainer = new drwAspectRatioWidget( m_mainWidget );
    drawingAreaLayout->addWidget( m_drawingWidgetContainer );
	
	// Create Drawing window
    m_glWidget = new drwDrawingWidget( m_drawingWidgetContainer );
    m_drawingWidgetContainer->setClientWidget( m_glWidget );
    m_glWidget->setMinimumSize( 480, 270 );
	m_glWidget->SetCurrentScene( m_scene );
    m_glWidget->SetToolbox( m_localToolbox );
    m_glWidget->SetControler( m_controler );
    m_globalLineParams->SetDrawingWidget( m_glWidget );
    m_linesApp->SetDrawingWidget( m_glWidget );

    m_localToolbox->SetRenderer( m_glWidget->GetRenderer() );

    // Create special widget to go inside drawing widget
    drwLineTool * lineTool = dynamic_cast<drwLineTool*>(m_localToolbox->GetTool( 0 ));
    Q_ASSERT(lineTool);
    m_viewportWidget = new drwLineToolViewportWidget( m_glWidget, lineTool );
    m_glWidget->SetViewportWidget( m_viewportWidget );
    m_cursor = new drwCursor( lineTool, m_glWidget );
    m_glWidget->SetCursor( m_cursor );
	
	// Create playback control widget
    m_playbackControlerWidget = new PlaybackControlerWidget( m_glWidget->GetControler(), m_mainWidget );
	drawingAreaLayout->addWidget( m_playbackControlerWidget );
    m_playbackControlerWidget->SetHideFrameRate( true );

	// Alternative right panel
    m_toolOptionWidget = new PrimitiveToolOptionWidget( lineTool, m_rightPanelWidget );
	rightPanelLayout->addWidget( m_toolOptionWidget );
    m_displaySettingsWidget = new DisplaySettingsWidget( m_glWidget, m_rightPanelWidget );
	rightPanelLayout->addWidget( m_displaySettingsWidget );

    QHBoxLayout * networkStateLayout = new QHBoxLayout();
    rightPanelLayout->addLayout( networkStateLayout );
    networkStateLayout->setContentsMargins ( 5, 5, 5, 5 );

    m_networkStateLabel = new QLabel( m_mainWidget );
    m_networkStateLabel->setText( "Not Connected" );
    m_networkStateLabel->setWordWrap( true );
    m_networkStateLabel->setFrameShape( QLabel::Box );
    m_networkStateLabel->setFrameShadow( QLabel::Plain );
    m_networkStateLabel->setAlignment( Qt::AlignCenter  );
    m_networkStateLabel->setMaximumWidth( 150 );
    m_networkStateLabel->setStyleSheet("background-color: #6e6e6e");
    networkStateLayout->addWidget( m_networkStateLabel );

	rightPanelLayout->addStretch();
	
	// Create tablet state dock
	m_dockTabletState = new QDockWidget(tr("Tablet State"));
	m_tabletStateWidget = new TabletStateWidget( m_dockTabletState );
    m_tabletStateWidget->SetDrawingWiget( m_glWidget );
	m_dockTabletState->setWidget(m_tabletStateWidget);
	m_viewMenu->addAction(m_dockTabletState->toggleViewAction());
	m_dockTabletState->setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable );
	
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
    delete m_linesApp;
}

void MainWindow::CreateActions()
{
	// Creates a file menu
    QMenu * file = menuBar()->addMenu( "&File" );
    m_fileNewAction = file->addAction( "New", this, SLOT( fileNew() ), Qt::CTRL + Qt::Key_N );
    m_fileOpenAction = file->addAction( "Open...", this, SLOT( fileOpen() ), Qt::CTRL + Qt::Key_O );
	file->addAction( "Save", this, SLOT( fileSave() ), Qt::CTRL + Qt::Key_S );
	file->addAction( "Save As...", this, SLOT( fileSaveAs() ), Qt::SHIFT + Qt::CTRL + Qt::Key_S );
	file->addAction( "Export...", this, SLOT( fileExport() ) );
    file->addAction( "&Exit", this, SLOT( close() ) );
    connect( file, SIGNAL(aboutToShow()), this, SLOT(fileMenuAboutToShow()) );
	
	// Create the Edit menu
	m_editMenu = menuBar()->addMenu( "&Edit" );
    m_editSetNumberOfFramesAction = m_editMenu->addAction( "Set Number of Frames", this, SLOT( editSetNumberOfFrames() ), Qt::CTRL + Qt::Key_G );
    //m_whiteOnBlackAction = new QAction( "White on black", m_editMenu );
    //m_whiteOnBlackAction->setCheckable( true );
    //m_whiteOnBlackAction->setChecked( true );
    //connect( m_whiteOnBlackAction, SIGNAL(toggled(bool)), this, SLOT(editWhiteOnBlackToggled(bool)) );
    //m_editMenu->addAction( m_whiteOnBlackAction );
    connect( m_editMenu, SIGNAL(aboutToShow()), this, SLOT(editMenuAboutToShow()) );
	
	// Create the Network menu
	m_networkMenu = menuBar()->addMenu( "&Network" );
	m_netShareSessionMenuAction = m_networkMenu->addAction( "Share session", this, SLOT( NetShareSession() ), Qt::CTRL + Qt::Key_T );
	m_netConnectMenuItem = m_networkMenu->addAction( "Connect...", this, SLOT( NetConnect() ) );
	
	// Create the View menu
    m_viewMenu = menuBar()->addMenu( "&View" );
    //m_viewMenu->addAction( "Fullscreen", this, SLOT( viewFullscreen() ), Qt::CTRL + Qt::Key_F );
	
    // Create a Help menu
    menuBar()->addSeparator();
    QMenu * help = menuBar()->addMenu( "&Help" );
    help->addSeparator();
    help->addAction( "&About", this, SLOT(about()) );
}


// -------- File Menu implementation --------

void MainWindow::fileMenuAboutToShow()
{
    bool networkOn = m_networkManager->IsConnected() || m_networkManager->IsSharing();
    m_fileNewAction->setEnabled( !m_networkManager->IsConnected() );
    m_fileOpenAction->setEnabled( !networkOn );
}

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
    m_commandDb->Read( m_filename.toUtf8().data() );

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
	
    m_commandDb->Write( m_filename.toUtf8().data() );
	return true;
}

void MainWindow::fileSaveAs()
{
	if( !GetSaveFilename() )
		return;
	
    m_commandDb->Write( m_filename.toUtf8().data() );
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
		exporter->SetSize( m_exportRes );
        exporter->SetExportAlpha( dlg.IsExportingAlpha() );
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
		}

        return res;
	}
	return false;
}

void MainWindow::editMenuAboutToShow()
{
    bool networkOn = m_networkManager->IsConnected();
    m_editSetNumberOfFramesAction->setEnabled( !networkOn );
}

void MainWindow::editSetNumberOfFrames()
{
	int nbFrames = QInputDialog::getInt( this, "Set Number of Frames" , "Number of Frames", m_scene->GetNumberOfFrames(), 0 );
	m_scene->SetNumberOfFrames( nbFrames );
}

void MainWindow::editWhiteOnBlackToggled( bool wob )
{
    SetWhiteOnBlack( wob );
    m_glWidget->NeedRedraw();
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
    if( m_networkManager->IsConnected() )
    {
        m_networkManager->Disconnect();
    }
    else
    {
        // Get the user to select the server to connect to
        drwNetworkConnectDialog * dlg = new drwNetworkConnectDialog( this );
        int result = dlg->exec();
        QString name;
        QHostAddress address;
        if( result != QDialog::Accepted || !dlg->GetSelectedUserAndAddress( name, address ) )
        {
            delete dlg;
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
        m_progressDialog->setLabelText("Waiting for server...");
        m_progressDialog->exec();

        m_scene->blockSignals( false );

        if( m_progressDialog->wasCanceled() )
        {
            m_networkManager->Disconnect();
        }

        delete m_progressDialog;
        m_progressDialog = 0;
    }
}

void MainWindow::NetConnectProgress()
{
	drwNetworkManager::NetworkState state = m_networkManager->GetState();
	if( state == drwNetworkManager::ReceivingScene )
	{
		// get progress and set it
		double percent = m_networkManager->GetPercentRead();
		m_progressDialog->setValue( percent );
		m_progressDialog->setLabelText("Receiving animation...");
	}
}

void MainWindow::UpdateNetworkStatus()
{
	if( m_networkManager->IsSharing() )
	{
		m_netShareSessionMenuAction->setText( tr("Stop Sharing" ) );
		m_netConnectMenuItem->setEnabled( false );
        m_networkStateLabel->setText( tr("Sharing") );
        m_networkStateLabel->setStyleSheet("background-color: #b16b2c");
	}
	else 
	{
        m_netShareSessionMenuAction->setText( tr("Share Session") );
		m_netConnectMenuItem->setEnabled( true );
		if( m_networkManager->IsConnected() )
		{
			m_netConnectMenuItem->setText( tr("Disconnect") );
			m_netShareSessionMenuAction->setEnabled( false );

            QString serverUserName = m_networkManager->GetServerUserName();
            m_networkStateLabel->setText( tr("Connected to: \n") + serverUserName );
            m_networkStateLabel->setStyleSheet("background-color: #526b2c");
		}
		else 
		{
            m_netShareSessionMenuAction->setEnabled( true );
			m_netConnectMenuItem->setText( tr("Connect...") );
            m_networkStateLabel->setText( tr("Not Connected") );
            m_networkStateLabel->setStyleSheet("background-color: #6e6e6e");
		}
	}
}

void MainWindow::NetStateChanged()
{
    drwNetworkManager::NetworkState state = m_networkManager->GetState();
    if( state == drwNetworkManager::ConnectionLost )
    {
        QString errorMessage = m_networkManager->GetErrorMessage();
        if( !errorMessage.isEmpty() )
            QMessageBox::warning( this, "Connection Lost", errorMessage );
        m_networkManager->ResetState();
        if( m_progressDialog )
            m_progressDialog->cancel();
    }
    else if( state == drwNetworkManager::ConnectionTimedOut )
    {
        QMessageBox::warning( this, "Server not responding", "Connection timed out!" );
        m_networkManager->ResetState();
        if( m_progressDialog )
            m_progressDialog->cancel();
    }
    else if( state == drwNetworkManager::Connected )
    {
        if( m_progressDialog )
            m_progressDialog->accept();
    }
    UpdateNetworkStatus();
}

void MainWindow::viewFullscreen()
{
	if( isFullScreen() )
	{
		showNormal();
        m_playbackControlerWidget->show();
        m_rightPanelDock->show();
        m_rightPanelDock->setFloating( false );
	}
	else
	{
        m_playbackControlerWidget->hide();
        m_rightPanelDock->hide();
        m_rightPanelDock->setFloating( true );
		showFullScreen();
	}
}

void MainWindow::toggleShowGui()
{
    m_guiHidden = !m_guiHidden;
    m_simplifiedToolbar->setHidden( m_guiHidden || !m_simplifiedGui );
    m_playbackControlerWidget->setHidden( m_guiHidden );
    m_rightPanelDock->setHidden( m_guiHidden || m_simplifiedGui );
    m_drawingWidgetContainer->setAspectRatioEnabled( !m_guiHidden );
    //m_glWidget->SetShowCameraFrame( !m_guiHidden );
}

void MainWindow::toggleRightPanel()
{
    bool isHidden = m_rightPanelDock->isHidden();
    if( isHidden )
    {
        m_rightPanelDock->show();
        m_playbackControlerWidget->SetHideFrameRate( false );
    }
    else
    {
        m_rightPanelDock->hide();
        m_playbackControlerWidget->SetHideFrameRate( true );
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

void MainWindow::SetWhiteOnBlack( bool wob )
{
    if( m_whiteOnBlack == wob )
        return;
    
    m_whiteOnBlack = wob;
    
    // update action
    m_whiteOnBlackAction->blockSignals( true );
    m_whiteOnBlackAction->setChecked( wob );
    m_whiteOnBlackAction->blockSignals( false );
    
    drwLineTool * lineTool = dynamic_cast<drwLineTool*>(m_localToolbox->GetTool( 0 ));
    Q_ASSERT(lineTool);
    
    // set new fg color as inverse of previous
    Vec4 currentFgCol = lineTool->GetColor();
    Vec4 newFgCol( 1.0 - currentFgCol[0], 1.0 - currentFgCol[1], 1.0 - currentFgCol[2], currentFgCol[3] );
    lineTool->SetColor( newFgCol );
    
    // Set new bg color depending on scheme
    if( m_whiteOnBlack )
    {
        Vec4 bg( 0.0, 0.0, 0.0, 1.0 );
        m_glWidget->SetBackgroundColor( bg );
    }
    else
    {
        Vec4 bg( 1.0, 1.0, 1.0, 1.0 );
        m_glWidget->SetBackgroundColor( bg );
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
    QSettings settings("SD", m_appName.toUtf8().data() );

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
    m_exportRes = settings.value( "ExportResolution", QSize( 1920, 1080 ) ).toSize();
    
    // white on black or black on white
    bool wob = settings.value( "WhiteOnBlack", true ).toBool();
    SetWhiteOnBlack( wob );
    
    // Allow toolbox and all tools to read their settings
    m_localToolbox->ReadSettings( settings );
}


void MainWindow::writeSettings()
{
	// first get out of fullscreen mode
	if( isFullScreen() )
		viewFullscreen();
	
    QSettings settings( "SD", m_appName.toUtf8().data() );
	
	// Main window settings
	settings.setValue( "MainWindow_pos", pos() );
	settings.setValue( "MainWindow_size", size() );
	
	// File dialog settings
	settings.setValue( "filedialogstartpath", m_fileDialogStartPath );
	settings.setValue( "ExportDefaultPath", m_exportDefaultPath );
	settings.setValue( "ExportResolution", m_exportRes );
    
    // white on black or black on white
    settings.setValue( "WhiteOnBlack", m_whiteOnBlack );
    
    // Allow toolbox and all tools to save their settings
    m_localToolbox->WriteSettings( settings );
}

void MainWindow::Reset()
{
	m_commandDispatcher->Reset();
	m_filename = "";
}

#include "linesversion.h"
#include "githash.h"

void MainWindow::about()
{
    QString msg = QString( "Lines - version %1.%2.%3\n" ).arg(LINES_MAJOR_VERSION).arg(LINES_MINOR_VERSION).arg(LINES_PATCH_VERSION);
    msg += QString("Protocol version %1\n").arg(LINES_PROTOCOL_VERSION);
    msg += QString("Git Hash: %1\n").arg(LINES_GIT_HASH_SHORT);
    msg += QString(LINES_COPYRIGHT);
    QMessageBox::about( this, m_appName, msg );
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
		else if ( keyEvent->key() == Qt::Key_Up )
		{
            m_glWidget->GetControler()->GotoStart();
			handled = true;
		}
		else if ( keyEvent->key() == Qt::Key_Down )
		{
            m_glWidget->GetControler()->GotoEnd();
			handled = true;
		}
        else if ( keyEvent->key() == Qt::Key_B )
        {
            m_linesApp->ToggleBigSmallBrush();
            m_glWidget->NeedRedraw();
            handled = true;
        }
        else if ( keyEvent->key() == Qt::Key_M )
        {
            m_linesApp->IncreaseBrushSize();
            m_glWidget->NeedRedraw();
            handled = true;
        }
        else if ( keyEvent->key() == Qt::Key_N )
        {
            m_linesApp->DecreaseBrushSize();
            m_glWidget->NeedRedraw();
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_Alt )
        {
            m_glWidget->ActivateViewportWidget( true );
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_Shift )
        {
            if( !m_linesApp->IsErasing() )
            {
                m_eraseToggled = true;
                m_linesApp->ToggleErasing();
                m_glWidget->NeedRedraw();
            }
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_H )
        {
            ToggleShowGlobalLineParams();
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_F )
        {
            toggleShowGui();
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_A )
        {
            toggleRightPanel();
            handled = true;
        }
        /*else if( keyEvent->key() == Qt::Key_T )
        {
            DrawSinusoidLine();
            handled = true;
        }*/
	}
    else if( event->type() == QEvent::KeyRelease )
    {
        QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
        if( keyEvent->key() == Qt::Key_Alt )
        {
            m_glWidget->ActivateViewportWidget( false );
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_Shift )
        {
            if( m_linesApp->IsErasing() && m_eraseToggled )
            {
                m_linesApp->ToggleErasing();
                m_eraseToggled = false;
            }
            m_glWidget->NeedRedraw();
            handled = true;
        }
    }
	if( !handled )
	{
		// standard event processing
		return QObject::eventFilter(obj, event);
	}
	return true;
}

void MainWindow::ToggleShowGlobalLineParams()
{
    if( m_globalLineParamsDock )
    {
        m_globalLineParamsDock->close();
        m_globalLineParamsDock = 0;
    }
    else
    {
        m_globalLineParamsDock = new QDockWidget( tr("GlobalLineParamsDock"), this );
        m_globalLineParamsDock->setAllowedAreas( Qt::NoDockWidgetArea );
        m_globalLineParamsDock->setFloating( true );
        QWidget * globalLineParamsWidget = m_globalLineParams->CreateGui();
        m_globalLineParamsDock->setWidget(globalLineParamsWidget);
        m_globalLineParamsDock->setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable );
        m_globalLineParamsDock->setAttribute( Qt::WA_DeleteOnClose );
        connect( m_globalLineParamsDock, SIGNAL(destroyed()), this, SLOT(GlobalLineParamsDockClosed()) );
        m_globalLineParamsDock->show();
    }
}

void MainWindow::GlobalLineParamsDockClosed()
{
    m_globalLineParamsDock = 0;
}

void MainWindow::DrawSinusoidLine()
{
    double xstart = 100.0;
    double ystart = 560.0;
    double interval = 5.0;
    double amp = 200.0;
    double per = 150;
    m_glWidget->SimulateTabletEvent( drwMouseCommand::Press, xstart, ystart, 0.0 );
    double x = xstart;
    double y = 0.0;
    while( x < 1800.0 )
    {
        double y = ystart + amp * sin( (x - xstart) / per * 6.2 );
        double pressure = std::abs( sin( (x - xstart) / per * 6.2 ) );
        m_glWidget->SimulateTabletEvent( drwMouseCommand::Move, x, y, pressure );
        QApplication::processEvents();
        x += interval;
    }
    m_glWidget->SimulateTabletEvent( drwMouseCommand::Release, x, y, 1.0 );
}
