#include "TabletStateWidget.h"


TabletStateWidget::TabletStateWidget( QWidget * parent ) : QWidget( parent )
{
	ui.setupUi(this);
	QTabletEvent event( QEvent::TabletEnterProximity, QPoint(0,0), QPoint(0,0), QPointF( 0, 0 ), QTabletEvent::NoDevice, QTabletEvent::UnknownPointer, 0, 0, 0, 0, 0, 0, 0, 0 );
	UpdateUi(&event);
}

TabletStateWidget::~TabletStateWidget()
{
}

void TabletStateWidget::tabletEvent( QTabletEvent * e )
{
	UpdateUi( e );
	e->ignore();
}

void TabletStateWidget::UpdateUi( QTabletEvent * e )
{
	if( e->type() == QEvent::TabletPress )
	{
		ui.PressStateLabel->setText( "Pressed" );
	}
	else if( e->type() == QEvent::TabletRelease )
	{
		ui.PressStateLabel->setText( "Released" );
	}
	else if( e->type() == QEvent::TabletEnterProximity )
	{
		ui.ProximityStateLabel->setText( "In Proximity" );
	}
	else if( e->type() == QEvent::TabletLeaveProximity )
	{
		ui.ProximityStateLabel->setText( "Out of Proximity" );
	}
	
	QString text;
	QString pType;
	switch ( e->pointerType() ) 
	{
		case QTabletEvent::UnknownPointer:
			pType = "QTabletEvent::UnknownPointer";
			break;
		case QTabletEvent::Pen:
			pType = "QTabletEvent::Pen";
			break;
		case QTabletEvent::Cursor:
			pType = "QTabletEvent::Cursor";
			break;
		case QTabletEvent::Eraser:
			pType = "QTabletEvent::Eraser";
			break;
		default:
			pType = "NONE";
			break;
	}
	text += QString("Pointer type:         ") + pType + "\n";
	QString tabletDevice;
	switch ( e->device() ) 
	{
		case QTabletEvent::NoDevice:
			tabletDevice = "QTabletEvent::NoDevice";
			break;
		case QTabletEvent::Puck:
			tabletDevice = "QTabletEvent::Puck";
			break;
		case QTabletEvent::Stylus:
			tabletDevice = "QTabletEvent::Stylus";
			break;
		case QTabletEvent::Airbrush:
			tabletDevice = "QTabletEvent::Airbrush";
			break;
		case QTabletEvent::FourDMouse:
			tabletDevice = "QTabletEvent::FourDMouse";
			break;
		case QTabletEvent::RotationStylus:
			tabletDevice = "QTabletEvent::RotationStylus";
			break;
		default:
			tabletDevice = "NONE";
			break;
	}
	text += QString("Tablet device:        ") + tabletDevice + "\n";
	text += QString("Position:             ( %1, %2, %3 )\n" ).arg( e->x()).arg( e->y() ).arg( e->z() );
	text += QString("Global Pos:           ( %1, %2 )\n" ).arg( e->globalX() ).arg( e->globalY() );
	text += QString("Hi res global Pos:    ( %1, %2 )\n" ).arg( e->hiResGlobalX() ).arg( e->hiResGlobalY() );
	text += QString("Tilt:                 ( %1, %2 )\n" ).arg( e->xTilt() ).arg( e->yTilt() );
	text += QString("Pressure:             %1\n" ).arg( e->pressure() );
	text += QString("Rotation:             %1\n" ).arg( e->rotation() );
	text += QString("Tangential pressure:  %1\n" ).arg( e->tangentialPressure() );
	ui.EventDescriptionTextEdit->setPlainText( text );
}
