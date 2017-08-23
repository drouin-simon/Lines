#ifndef __drwSimplifiedToobar_h_
#define __drwSimplifiedToobar_h_

#include <QWidget>
#include <QPixmap>

class LinesApp;

namespace Ui {
class drwSimplifiedToolbar;
}

class drwSimplifiedToolbar : public QWidget
{
    Q_OBJECT

public:

    explicit drwSimplifiedToolbar(QWidget *parent = 0);
    ~drwSimplifiedToolbar();

    void SetApp( LinesApp * app );
    void SetOrientationToVertical();
    void SetOrientationToHorizontal();

protected slots:

    void UpdateUI();

private slots:

    void on_smallBrushButton_toggled(bool checked);
    void on_eraserButton_toggled(bool checked);
    void on_opacityWidget_sliderValueChanged( double val );
    void on_manualModeButton_toggled(bool checked);
    void on_jumpModeButton_toggled(bool checked);
    void on_oneOnionButton_toggled(bool checked);
    void on_pressureWidthButton_toggled(bool checked);
    void on_pressureOpacityButton_toggled(bool checked);
    void on_fillButton_toggled(bool checked);

    void on_onionBeforeSpinBox_valueChanged(int arg1);
    void on_onionAfterSpinBox_valueChanged(int arg1);
    void on_playModeButton_toggled(bool checked);

    void on_persistenceSpinBox_valueChanged(int arg1);

private:

    void BlockSigs( bool block );

    Ui::drwSimplifiedToolbar *ui;
    LinesApp * m_app;
    bool m_vertical;

    QPixmap m_notConnectedIcon;
    QPixmap m_connectedIcon;
    QPixmap m_sharingIcon;
};

#endif
