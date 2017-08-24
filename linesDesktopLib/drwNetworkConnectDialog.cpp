#include "drwNetworkConnectDialog.h"
#include "ui_drwNetworkConnectDialog.h"
#include "drwNetworkServerFinder.h"

drwNetworkConnectDialog::drwNetworkConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::drwNetworkConnectDialog),
    m_selectedRow( -1 )
{
    m_serverName = QString( "Manual IP" );
    ui->setupUi(this);
    m_finder = new drwNetworkServerFinder( );
    connect( m_finder, SIGNAL(ModifiedSignal()), this, SLOT(UpdateUi()) );
    UpdateUi();
}

drwNetworkConnectDialog::~drwNetworkConnectDialog()
{
    delete m_finder;
    delete ui;
}

void drwNetworkConnectDialog::SetServerAddress( QHostAddress & address )
{
    m_serverAddress = address;
    UpdateUi();
}

void drwNetworkConnectDialog::UpdateUi()
{
    const QStringList & names = m_finder->GetServerUsers();
    const QList<QHostAddress> & addresses = m_finder->GetServerAddresses();

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount( names.size() );
    for( int i = 0; i < names.size(); ++i )
    {
        QTableWidgetItem * nameItem = new QTableWidgetItem( names.at( i ) );
        ui->tableWidget->setItem( i, 0, nameItem );
        QTableWidgetItem * addressItem = new QTableWidgetItem( addresses.at( i ).toString() );
        ui->tableWidget->setItem( i, 1, addressItem );
    }

    if( m_selectedRow != -1 )
    {
        QTableWidgetSelectionRange range( m_selectedRow, 0, m_selectedRow, 1 );
        ui->tableWidget->setRangeSelected( range, true );
    }

    ui->serverIPEdit->blockSignals( true );
    ui->serverIPEdit->setText( m_serverAddress.toString() );
    ui->serverIPEdit->blockSignals( false );
}

void drwNetworkConnectDialog::on_tableWidget_cellPressed(int row, int column)
{
    m_selectedRow = row;

    const QList<QHostAddress> & addresses = m_finder->GetServerAddresses();
    Q_ASSERT( addresses.size() > row );
    m_serverAddress = addresses[ row ];

    UpdateUi();
}

void drwNetworkConnectDialog::on_serverIPEdit_textChanged(const QString &arg1)
{
    m_serverAddress.setAddress( arg1 );
}
