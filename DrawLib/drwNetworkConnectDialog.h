#ifndef __drwNetworkConnectDialog_h_
#define __drwNetworkConnectDialog_h_

#include <QDialog>

class QHostAddress;
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
	
	bool GetSelectedUserAndAddress( QString & user, QHostAddress & address );

public slots:

    void UpdateUi();

private:
    Ui::drwNetworkConnectDialog *ui;

private slots:
    void on_tableWidget_cellPressed(int row, int column);
   
protected:

    drwNetworkServerFinder * m_finder;
    int m_selectedRow;
};

#endif // CONNECTWIDGET_H
