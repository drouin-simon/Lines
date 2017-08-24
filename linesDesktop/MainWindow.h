#ifndef __MainWindow_h_
#define __MainWindow_h_

#include <QMainWindow>
#include <QString>
#include <QHostAddress>

class QPopupMenu;
class QWorkspace;
class QPopupMenu;
class QDockWidget;
class QAction;
class QProgressDialog;
class QLabel;

class drwAspectRatioWidget;
class drwDrawingWidget;
class PrimitiveToolOptionWidget;
class PlaybackControlerWidget;
class TabletStateWidget;
class DisplaySettingsWidget;
class drwNetworkManager;
class drwGlobalLineParams;
class drwSimplifiedToolbar;
class SideToolbar;
class LinesApp;
class LinesCore;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow();
    ~MainWindow();
	
	void closeEvent( QCloseEvent * e );

private slots:
	
    void fileMenuAboutToShow();
	void fileNew();
	void fileOpen();
	bool fileSave();
	void fileSaveAs();
	bool fileExport();
	
    void editMenuAboutToShow();
	void editSetNumberOfFrames();
    void editWhiteOnBlackToggled( bool wob );
	
	void NetShareSession();
	void NetConnect();
	void NetConnectProgress();
    void NetStateChanged();
	void UpdateNetworkStatus();
	
	void viewFullscreen();
    void toggleShowGui();

    void about();
    
    void GlobalLineParamsDockClosed();
	
protected:
	
	// Get global keyboard shortcuts
	bool eventFilter(QObject *obj, QEvent *event);
	
	// Construction utilities
	void CreateActions();

private:
	
    void SetWhiteOnBlack( bool wob );
	bool GetSaveFilename();
	bool maybeSave();
	void readSettings();
	void writeSettings();
	void Reset();
    void ToggleShowGlobalLineParams();
    void DrawSinusoidLine();
	
	QString m_filename;
	QString m_fileDialogStartPath;
	
	QString m_exportDefaultPath;
	QSize m_exportRes;

    drwAspectRatioWidget * m_drawingWidgetContainer;
	drwDrawingWidget    * m_glWidget;
    drwSimplifiedToolbar * m_simplifiedToolbar;
    SideToolbar * m_sideToolbar;
    LinesApp * m_linesApp;

	QMenu       * m_viewMenu;
	QMenu		* m_editMenu;
	QMenu		* m_networkMenu;
	
    QAction * m_fileNewAction;
    QAction * m_fileOpenAction;
    QAction * m_editSetNumberOfFramesAction;
    QAction * m_whiteOnBlackAction;
	QAction * m_netShareSessionMenuAction;
	QAction * m_netConnectMenuItem;
	
    // Right panel
    QWidget * m_mainWidget;
	PlaybackControlerWidget * m_playbackControlerWidget;

	TabletStateWidget * m_tabletStateWidget;
	QDockWidget * m_dockTabletState;
    
    drwGlobalLineParams * m_globalLineParams;
    QDockWidget * m_globalLineParamsDock;
	
    LinesCore * m_lines;

    // Network
    QHostAddress m_lastServerAddress;
    drwNetworkManager * m_networkManager;

	QProgressDialog * m_progressDialog;

    static const QString m_appName;
    bool m_whiteOnBlack;
    bool m_eraseToggled;
    bool m_guiHidden;
};


#endif
