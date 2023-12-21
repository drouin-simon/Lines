#include "MainWindow.h"
#include "SideToolbar.h"
#include "drwDrawingWidget.h"
#include "drwAspectRatioWidget.h"
#include "PlaybackControlerWidget.h"
#include "TabletStateWidget.h"
#include "ExportDialog.h"
#include "drwNetworkConnectDialog.h"
#include "drwNetworkManager.h"
#include "drwBitmapExporter.h"
#include "drwGlobalLineParams.h"
#include <QtWidgets>
#include "drwsimplifiedtoolbar.h"
#include "LinesApp.h"
#include "LinesCore.h"
#include "LinesPreferencesWidget.h"
#include "../GraphicsEngine/include/GraphicsEngineManager.h"

const QString MainWindow::m_appName( "Lines" );

MainWindow::MainWindow()
	: m_progressDialog(0)
{
    setWindowTitle( m_appName );
    m_whiteOnBlack = true;
    m_eraseToggled = false;
    m_guiHidden = false;

	CreateActions();

    // Create main interface with linesCore
    m_lines = new LinesCore;

    // Create the network manager and connect it to the core
	m_networkManager = new drwNetworkManager();
    m_networkManager->SetLinesCore( m_lines );
    m_lines->SetRemoteIO( m_networkManager );
    connect( m_networkManager, SIGNAL(StateChangedSignal()), this, SLOT(NetStateChanged()) );

    m_globalLineParams = new drwGlobalLineParams( this );
    m_globalLineParamsDock = 0;

    m_linesPreferences = 0;

    m_linesApp = new LinesApp( m_lines, m_networkManager );

	// Create main widget  (just a frame to put the viewing widget and the playback control widget)
    m_mainWidget = new QWidget(this);
    setCentralWidget( m_mainWidget );

    m_mainLayout = new QHBoxLayout( m_mainWidget );
    m_mainLayout->setContentsMargins( 0, 0, 0, 0 );
    m_mainLayout->setSpacing( 0 );

    // Side toolbar (only left for now)
    m_sideToolbarLeft = true;
    m_sideToolbar = new SideToolbar( m_mainWidget );
    m_sideToolbar->SetApp( m_linesApp );
	m_sideToolbar->SetButtonWidth(50);
    m_mainLayout->addWidget( m_sideToolbar );

    // Drawing area layout (drawing window + timeline)
    m_drawingAreaLayout = new QVBoxLayout();
    m_drawingAreaLayout->setContentsMargins( 0, 0, 0, 0 );
    m_mainLayout->addLayout( m_drawingAreaLayout );

    // Set default OpenGL surface format that will be applied to all openGL windows
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    //format.setVersion(3, 2);
    //format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
    QSurfaceFormat::setDefaultFormat(format);

    // Toolbar
    m_simplifiedToolbar = new drwSimplifiedToolbar( m_mainWidget );
    m_simplifiedToolbar->SetApp( m_linesApp );
	m_simplifiedToolbar->SetButtonHeight(30);
    m_drawingAreaLayout->addWidget( m_simplifiedToolbar );

    // Create Drawing window container that maintains a 16:9 aspect ratio
    m_drawingWidgetContainer = new drwAspectRatioWidget( m_mainWidget );
    m_drawingAreaLayout->addWidget( m_drawingWidgetContainer );
	
	// Create Drawing window
    m_glWidget = new drwDrawingWidget( m_drawingWidgetContainer );
    m_glWidget->SetLinesCore( m_lines );
    m_lines->SetDrawingSurface( m_glWidget );
    m_drawingWidgetContainer->setClientWidget( m_glWidget );
    m_glWidget->setMinimumSize( 480, 270 );
    m_globalLineParams->SetLinesCore( m_lines );
	
	// Create playback control widget
    m_playbackControlerWidget = new PlaybackControlerWidget( m_lines, m_mainWidget );
    m_drawingAreaLayout->addWidget( m_playbackControlerWidget );
	
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
    GraphicsEngineManager::deleteGraphicsEngine();
    delete m_linesApp;
    delete m_lines;
}

void MainWindow::SetShowSideToolbar( bool show )
{
    m_sideToolbar->setHidden( !show );
}

bool MainWindow::IsSideToolbarShown()
{
    return !m_sideToolbar->isHidden();
}

void MainWindow::SetSideToolbarLeft( bool left )
{
    m_sideToolbarLeft = left;
    m_mainLayout->removeWidget( m_sideToolbar );
    m_mainLayout->removeItem( m_drawingAreaLayout );
    if( left )
    {
        m_mainLayout->addWidget( m_sideToolbar );
        m_mainLayout->addItem( m_drawingAreaLayout );
    }
    else
    {
        m_mainLayout->addItem( m_drawingAreaLayout );
        m_mainLayout->addWidget( m_sideToolbar );
    }
}

bool MainWindow::IsSideToolbarLeft()
{
    return m_sideToolbarLeft;
}

void MainWindow::SetSideToolbarWidth( int w )
{
    m_sideToolbar->SetButtonWidth( w );
}

int MainWindow::GetSideToolbarWidth()
{
    return m_sideToolbar->GetButtonWidth();
}

void MainWindow::SetMainToolbarHeight( int h )
{
    m_simplifiedToolbar->SetButtonHeight( h );
}

int MainWindow::GetMainToolbarHeight()
{
    return m_simplifiedToolbar->GetButtonHeight();
}

void MainWindow::CreateActions()
{
	// Creates a file menu
    QMenu * file = menuBar()->addMenu( "&File" );
    m_fileNewAction = file->addAction( "New", this, SLOT( fileNew() ), Qt::CTRL + Qt::Key_N );
    m_fileOpenAction = file->addAction( "Open...", this, SLOT( fileOpen() ), Qt::CTRL | Qt::Key_O );
	file->addAction( "Save", this, SLOT( fileSave() ), Qt::CTRL + Qt::Key_S );
	file->addAction( "Save As...", this, SLOT( fileSaveAs() ), Qt::SHIFT | Qt::CTRL | Qt::Key_S );
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
    QAction * preferencesAction = m_editMenu->addAction( "preferences...", this, SLOT( editPreferences() ) );
	
	// Create the Network menu
	m_networkMenu = menuBar()->addMenu( "&Network" );
	m_netShareSessionMenuAction = m_networkMenu->addAction( "Share session", this, SLOT( NetShareSession() ), Qt::CTRL + Qt::Key_T );
	m_netConnectMenuItem = m_networkMenu->addAction( "Connect...", this, SLOT( NetConnect() ) );
	
	// Create the View menu
    m_viewMenu = menuBar()->addMenu( "&View" );
#ifdef Q_OS_WIN
	m_viewMenu->addAction("Fullscreen", this, SLOT(viewFullscreen()), Qt::CTRL + Qt::Key_F);
#endif // Q_OS_WIN
	
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
    m_fileNewAction->setEnabled( !m_networkManager->IsConnected() || m_linesApp->IsMaster() );
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
	
    // Load animation
    m_lines->LoadAnimation( m_filename.toUtf8().data() );
}

bool MainWindow::fileSave()
{
	if( m_filename.isEmpty() )
	{
		if( !GetSaveFilename() )
			return false;
	}
	
    m_lines->SaveAnimation( m_filename.toUtf8().data() );

	return true;
}

void MainWindow::fileSaveAs()
{
	if( !GetSaveFilename() )
		return;
	
    m_lines->SaveAnimation( m_filename.toUtf8().data() );
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
        exporter->SetScene( m_lines->GetScene() );
		exporter->SetSize( m_exportRes );
        exporter->SetExportAlpha( dlg.IsExportingAlpha() );
		bool res = exporter->StartWriting();
		
		// Create the progress dialog, connect it to the exporter and start modal
		if( res )
		{
			QProgressDialog dialog;
			dialog.setLabelText("Exporting animation to bitmap");
            dialog.setRange( 0, m_lines->GetNumberOfFrames() - 1 );
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
    m_editSetNumberOfFramesAction->setEnabled( !networkOn || m_linesApp->IsMaster() );
}

void MainWindow::editSetNumberOfFrames()
{
    int nbFrames = QInputDialog::getInt( this, "Set Number of Frames" , "Number of Frames", m_lines->GetNumberOfFrames(), 0 );
    m_lines->SetNumberOfFrames( nbFrames );
}

void MainWindow::editWhiteOnBlackToggled( bool wob )
{
    SetWhiteOnBlack( wob );
    m_glWidget->NeedRedraw();
}

void MainWindow::editPreferences()
{
    if( !m_linesPreferences )
    {
        m_linesPreferences = new LinesPreferencesWidget;
        m_linesPreferences->setAttribute( Qt::WA_DeleteOnClose );
        Qt::WindowFlags flags = Qt::Tool;
        m_linesPreferences->setWindowFlags( flags );
        connect( m_linesPreferences, SIGNAL(destroyed(QObject*)), this, SLOT(OnLinesPreferencesClosed()) );
        m_linesPreferences->SetMainWindow( this );
    }
    m_linesPreferences->show();
}

void MainWindow::OnLinesPreferencesClosed()
{
    m_linesPreferences = 0;
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

#include <QHostAddress>

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
        dlg->SetServerAddress( m_lastServerAddress );
        int result = dlg->exec();
        if( result != QDialog::Accepted || dlg->GetServerAddress().isNull() )
        {
            delete dlg;
            return;
        }
        m_lastServerAddress = dlg->GetServerAddress();
        QString serverName = dlg->GetServerUserName();
        delete dlg;

        // Now we have a valid username and ip, try to connect
        Reset();
        m_lines->EnableRendering( false );
        m_networkManager->Connect( serverName, m_lastServerAddress );

        // Create a timer to update a progress dialog
        QTimer timer;
        connect( &timer, SIGNAL(timeout()), this, SLOT(NetConnectProgress()) );
        timer.start( 100 );

        // Create progress dialog and start it
        m_progressDialog = new QProgressDialog( "Connecting", "Cancel", 0, 100 );
        m_progressDialog->setLabelText("Waiting for server...");
        m_progressDialog->exec();

        m_lines->EnableRendering( true );

        if( m_progressDialog->wasCanceled() )
        {
            m_networkManager->Disconnect();
        }

        // make sure other nodes in the network know the state of local session
        m_lines->EmitStateCommands();

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
		}
		else 
		{
            m_netShareSessionMenuAction->setEnabled( true );
			m_netConnectMenuItem->setText( tr("Connect...") );
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
	}
	else
	{
		showFullScreen();
	}
}

void MainWindow::toggleShowGui()
{
    m_guiHidden = !m_guiHidden;
    m_simplifiedToolbar->setHidden( m_guiHidden );
    m_sideToolbar->setHidden( m_guiHidden );
    m_playbackControlerWidget->setHidden( m_guiHidden );
    m_drawingWidgetContainer->setAspectRatioEnabled( !m_guiHidden );
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
    
    drwLineTool * lineTool = m_lines->GetLineTool();
    Q_ASSERT(lineTool);
    
    // set new fg color as inverse of previous
    Vec4 currentFgCol = lineTool->GetColor();
    Vec4 newFgCol( 1.0 - currentFgCol[0], 1.0 - currentFgCol[1], 1.0 - currentFgCol[2], currentFgCol[3] );
    lineTool->SetColor( newFgCol );
    
    // Set new bg color depending on scheme
    if( m_whiteOnBlack )
    {
        m_lines->SetBackgroundColor( 0.0, 0.0, 0.0, 1.0 );
    }
    else
    {
        m_lines->SetBackgroundColor( 1.0, 1.0, 1.0, 1.0 );
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
    if( m_lines->IsAnimationModified() )
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
	int mainToolbarHeight = settings.value("MainToolbarHeight", 30).toInt();
	SetMainToolbarHeight(mainToolbarHeight);
	int sideToolbarWidth = settings.value("SideToolbarWidth", 50).toInt();
	SetSideToolbarWidth(sideToolbarWidth);
	bool sideToolbarLeft = settings.value("SideToolbarLeft", true).toBool();
	SetSideToolbarLeft(sideToolbarLeft);
	bool showSideToolbar = settings.value("ShowSideToolbar", true).toBool();
	SetShowSideToolbar(showSideToolbar);
	
	// Save path
	m_fileDialogStartPath = settings.value( "filedialogstartpath", QDir::homePath() ).toString();
	
	// Export settings
	m_exportDefaultPath = settings.value( "ExportDefaultPath", QDir::homePath() ).toString();
    m_exportRes = settings.value( "ExportResolution", QSize( 1920, 1080 ) ).toSize();
    
    // white on black or black on white
    bool wob = settings.value( "WhiteOnBlack", true ).toBool();
    SetWhiteOnBlack( wob );

    // Last server address
    m_lastServerAddress = QHostAddress( settings.value( "LastServerAddress", QHostAddress().toString() ).toString() );

    // Allow toolbox and all tools to read their settings
    m_lines->ReadSettings( settings );
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

	settings.setValue("MainToolbarHeight", GetMainToolbarHeight());
	settings.setValue("SideToolbarWidth", GetSideToolbarWidth());
	settings.setValue("SideToolbarLeft", IsSideToolbarLeft());
	settings.setValue("ShowSideToolbar", IsSideToolbarShown());
	
	// File dialog settings
	settings.setValue( "filedialogstartpath", m_fileDialogStartPath );
	settings.setValue( "ExportDefaultPath", m_exportDefaultPath );
	settings.setValue( "ExportResolution", m_exportRes );
    
    // white on black or black on white
    settings.setValue( "WhiteOnBlack", m_whiteOnBlack );

    // Last server address
    settings.setValue( "LastServerAddress", m_lastServerAddress.toString() );
    
    // Allow toolbox and all tools to save their settings
    m_lines->WriteSettings( settings );
}

void MainWindow::Reset()
{
    m_lines->NewAnimation();
	m_filename = "";
}

#include "linesversion.h"
//#include "githash.h"

void MainWindow::about()
{
    QString msg = QString( "Lines - version %1.%2.%3\n" ).arg(LINES_MAJOR_VERSION).arg(LINES_MINOR_VERSION).arg(LINES_PATCH_VERSION);
    msg += QString("%1\n").arg(LINES_VERSION_QUALIFIER);
    msg += QString("Protocol version %1\n").arg(LINES_PROTOCOL_VERSION);
    //msg += QString("Git Hash: %1\n").arg(LINES_GIT_HASH_SHORT);
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
            m_lines->NextFrame();
			handled = true;
		}
		else if( keyEvent->key() == Qt::Key_Left )
		{
            m_lines->PrevFrame();
			handled = true;
		}
		else if( keyEvent->key() == Qt::Key_Space )
		{
            m_lines->PlayPause();
			handled = true;
		}
		else if ( keyEvent->key() == Qt::Key_Up )
		{
            m_lines->GotoStart();
			handled = true;
		}
		else if ( keyEvent->key() == Qt::Key_Down )
		{
            m_lines->GotoEnd();
			handled = true;
		}
        else if ( keyEvent->key() == Qt::Key_B )
        {
            m_linesApp->UseBrush();
            handled = true;
        }
        else if ( keyEvent->key() == Qt::Key_E )
        {
            m_linesApp->SetErasing();
            handled = true;
        }
        else if ( keyEvent->key() == Qt::Key_M )
        {
            m_linesApp->IncreaseBrushSize();
            handled = true;
        }
        else if ( keyEvent->key() == Qt::Key_N )
        {
            m_linesApp->DecreaseBrushSize();
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_S )
        {
            m_linesApp->ToggleOnionSkinEnabled();
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_Shift )
        {
            if( !m_linesApp->IsErasing() )
            {
                m_eraseToggled = true;
                m_linesApp->SetErasing();
            }
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_H )
        {
            ToggleShowGlobalLineParams();
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_A )
        {
            toggleShowGui();
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_I )
        {
            m_linesApp->ToggleInOnionSkin();
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_O )
        {
            m_linesApp->ToggleOutOnionSkin();
            handled = true;
        }
        else if( keyEvent->key() == Qt::Key_F )
        {
            m_linesApp->SetFlippingModeEnabled( true );
            handled = true;
        }
	}
    else if( event->type() == QEvent::KeyRelease )
    {
        QKeyEvent * keyEvent = static_cast<QKeyEvent *>(event);
        if( keyEvent->key() == Qt::Key_Shift )
        {
            if( m_linesApp->IsErasing() && m_eraseToggled )
            {
                m_linesApp->UseBrush();
                m_eraseToggled = false;
            }
            handled = true;
        }
        if( keyEvent->key() == Qt::Key_F )
        {
            m_linesApp->SetFlippingModeEnabled( false );
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
    m_lines->MouseEventWorld( drwMouseCommand::Press, xstart, ystart, 0.0 );
    double x = xstart;
    double y = 0.0;
    while( x < 1800.0 )
    {
        double y = ystart + amp * sin( (x - xstart) / per * 6.2 );
        double pressure = std::abs( sin( (x - xstart) / per * 6.2 ) );
        m_lines->MouseEventWorld( drwMouseCommand::Move, x, y, pressure );
        QApplication::processEvents();
        x += interval;
    }
    m_lines->MouseEventWorld( drwMouseCommand::Release, x, y, 1.0 );
}
