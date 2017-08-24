#ifndef __drwNetworkConnectDialog_h_
#define __drwNetworkConnectDialog_h_

#include <QDialog>
#include <QHostAddress>

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
	
    void SetServerAddress( QHostAddress & address );
    const QHostAddress & GetServerAddress() { return m_serverAddress; }
    const QString & GetServerUserName() { return m_serverName; }

public slots:

    void UpdateUi();

private:

    Ui::drwNetworkConnectDialog *ui;

private slots:

    void on_tableWidget_cellPressed(int row, int column);
    void on_serverIPEdit_textChanged(const QString &arg1);

protected:

    drwNetworkServerFinder * m_finder;
    int m_selectedRow;
    QHostAddress m_serverAddress;
    QString m_serverName;
};

#endif
