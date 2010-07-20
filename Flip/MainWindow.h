#ifndef __MainWindow_h_
#define __MainWindow_h_

#include <qmainwindow.h>
#include <qstring.h>

class QPopupMenu;
class QWorkspace;
class QPopupMenu;
class GlWidget;
class Scene;
class ImageFlippingTool;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow( QWidget * parent = 0, Qt::WFlags flags = 0 );
    ~MainWindow();

private slots:

    void about();
    void aboutQt();
    void fileSettings();

private:

    QWorkspace * m_workspace;
    QPopupMenu * m_windowsMenu;
	GlWidget   * m_glWidget;
	Scene      * m_scene;
	ImageFlippingTool * m_flippingTool;

    static const QString m_appName;
};


#endif
