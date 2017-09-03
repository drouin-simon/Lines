#ifndef __LinesPreferencesWidget_h_
#define __LinesPreferencesWidget_h_

#include <QWidget>

namespace Ui {
class LinesPreferencesWidget;
}

class MainWindow;

class LinesPreferencesWidget : public QWidget
{
    Q_OBJECT

public:

    explicit LinesPreferencesWidget(QWidget *parent = 0);
    ~LinesPreferencesWidget();

    void SetMainWindow( MainWindow * mw );

protected slots:

    void UpdateUI();

private slots:

    void on_sideToolbarGroupBox_toggled(bool arg1);
    void on_sideToolbarLeftRadioButton_toggled(bool checked);
    void on_sideToolbarRightRadioButton_toggled(bool checked);
    void on_sideToolbarWidthSpinBox_valueChanged(int arg1);
    void on_mainToolbarHeightSpinBox_valueChanged(int arg1);

private:

    Ui::LinesPreferencesWidget *ui;
    MainWindow * m_mainWindow;
};

#endif
