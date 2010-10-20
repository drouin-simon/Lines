#ifndef __drwNetworkConnectDialog_h_
#define __drwNetworkConnectDialog_h_

#include <QDialog>

class drwNetworkServerFinder;

namespace Ui {
    class drwNetworkConnectDialog;
}

class drwNetworkConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit drwNetworkConnectDialog(QWidget *parent = 0);
    ~drwNetworkConnectDialog();

public slots:

    void UpdateUi();

private:
    Ui::drwNetworkConnectDialog *ui;

private slots:
    void on_tableWidget_cellPressed(int row, int column);
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

protected:

    drwNetworkServerFinder * m_finder;
    int m_selectedRow;
};

#endif // CONNECTWIDGET_H
