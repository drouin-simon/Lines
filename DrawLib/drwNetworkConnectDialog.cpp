#include "drwNetworkConnectDialog.h"
#include "ui_drwNetworkConnectDialog.h"
#include "drwNetworkServerFinder.h"

drwNetworkConnectDialog::drwNetworkConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::drwNetworkConnectDialog),
    m_selectedRow( -1 )
{
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

bool drwNetworkConnectDialog::GetSelectedUserAndAddress( QString & user, QHostAddress & address )
{
	if( m_selectedRow != -1 && m_finder->GetServerUsers().size() > m_selectedRow )
	{
		const QStringList & names = m_finder->GetServerUsers();
		user = names.at( m_selectedRow );
		const QList<QHostAddress> & addresses = m_finder->GetServerAddresses();
		address = addresses.at( m_selectedRow );
		return true;
	}
	return false;
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
}

void drwNetworkConnectDialog::on_tableWidget_cellPressed(int row, int column)
{
    m_selectedRow = row;
    UpdateUi();
}
