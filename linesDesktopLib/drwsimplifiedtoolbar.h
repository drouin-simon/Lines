#ifndef __drwSimplifiedToobar_h_
#define __drwSimplifiedToobar_h_

#include <QWidget>

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

protected slots:

    void UpdateUI();

private slots:

    void on_smallBrushButton_toggled(bool checked);
    void on_eraserButton_toggled(bool checked);
    void on_opacityWidget_sliderValueChanged( double val );
    void on_manualModeButton_toggled(bool checked);
    void on_jumpModeButton_toggled(bool checked);
    void on_oneOnionButton_toggled(bool checked);
    void on_manyOnionButton_toggled(bool checked);
    void on_noOnionButton_toggled(bool checked);

private:

    void BlockSigs( bool block );

    Ui::drwSimplifiedToolbar *ui;
    LinesApp * m_app;
};

#endif
