#ifndef __SideToolbar_h_
#define __SideToolbar_h_

#include <QWidget>

namespace Ui {
class SideToolbar;
}

class LinesApp;

class SideToolbar : public QWidget
{
    Q_OBJECT

public:

    explicit SideToolbar(QWidget *parent = 0);
    ~SideToolbar();

    void SetApp( LinesApp * app );

private slots:

    void OnScrollWidgetClick(int);
    void on_nextFrameButton_clicked();
    void on_prevFrameButton_clicked();
    void on_increaseSizeButton_clicked();
    void on_decreaseSizeButton_clicked();

private:

    void UpdateUi();

    Ui::SideToolbar *ui;

    LinesApp * m_app;
};

#endif
