#ifndef __MainWindow_h_
#define __MainWindow_h_

#include <QMainWindow>
#include <QString>

class QPopupMenu;
class QWorkspace;
class QPopupMenu;
class QDockWidget;
class QAction;
class QProgressDialog;
class QLabel;

class drwDrawingWidget;
class Scene;
class drwLineTool;
class drwToolbox;
class drwCommandDatabase;
class PrimitiveToolOptionWidget;
class PlaybackControlerWidget;
class TabletStateWidget;
class DisplaySettingsWidget;
class PlaybackControler;
class drwCommandDispatcher;
class drwNetworkManager;
class drwLineToolViewportWidget;
class drwCursor;
class drwDrawingEngine;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow();
    ~MainWindow();
	
	void closeEvent( QCloseEvent * e );

private slots:
	
	void fileNew();
	void fileOpen();
	bool fileSave();
	void fileSaveAs();
	bool fileExport();
	
	void editSetNumberOfFrames();
    void editWhiteOnBlackToggled( bool wob );
	
	void NetShareSession();
	void NetConnect();
	void NetConnectProgress();
    void NetStateChanged();
	void UpdateNetworkStatus();
	
	void viewFullscreen();

    void about();
    void aboutQt();
	
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
	
	QString m_filename;
	QString m_fileDialogStartPath;
	
	QString m_exportDefaultPath;
	QSize m_exportRes;

	drwDrawingWidget    * m_glWidget;

	QMenu       * m_viewMenu;
	QMenu		* m_editMenu;
	QMenu		* m_networkMenu;
	
    QAction * m_whiteOnBlackAction;
	QAction * m_netShareSessionMenuAction;
	QAction * m_netConnectMenuItem;
	
    // Right panel
    QWidget * m_mainWidget;
    QDockWidget * m_rightPanelDock;
    QWidget * m_rightPanelWidget;
	PlaybackControlerWidget * m_playbackControlerWidget;
	PrimitiveToolOptionWidget * m_toolOptionWidget;
	DisplaySettingsWidget * m_displaySettingsWidget;
    QLabel * m_networkStateLabel;

	TabletStateWidget * m_tabletStateWidget;
	QDockWidget * m_dockTabletState;
	
	Scene					* m_scene;
	PlaybackControler		* m_controler;
	drwToolbox				* m_localToolbox;
	drwCommandDatabase		* m_commandDb;
    drwCommandDispatcher	* m_commandDispatcher;
	drwNetworkManager		* m_networkManager;
    drwLineToolViewportWidget * m_viewportWidget;
    drwCursor              * m_cursor;
    drwDrawingEngine       * m_drawingEngine;

	QProgressDialog * m_progressDialog;

    static const QString m_appName;
    bool m_whiteOnBlack;
};


#endif
