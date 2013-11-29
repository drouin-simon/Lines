#ifndef __MainWindow_h_
#define __MainWindow_h_

#include <QMainWindow>
#include <QString>

class QPopupMenu;
class QWorkspace;
class QPopupMenu;
class drwDrawingWidget;
class Scene;
class ImageFlippingTool;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow();
    ~MainWindow();

private slots:

    void about();
    void aboutQt();
    void fileSettings();

private:

    QWorkspace * m_workspace;
    QPopupMenu * m_windowsMenu;
    drwDrawingWidget * m_glWidget;
	Scene      * m_scene;
	ImageFlippingTool * m_flippingTool;

    static const QString m_appName;
};


#endif
