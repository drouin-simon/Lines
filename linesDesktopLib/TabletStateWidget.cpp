#include "TabletStateWidget.h"
#include "TabletStateTestingArea.h"

TabletStateWidget::TabletStateWidget( QWidget * parent ) : QWidget( parent )
{
	ui.setupUi(this);
    ui.openGLWidget->SetTabletStateWidget( this );
    QTabletEvent event( QEvent::TabletEnterProximity, QPoint(0,0), QPointF( 0, 0 ), QTabletEvent::NoDevice, QTabletEvent::UnknownPointer, 0, 0, 0, 0, 0, 0, 0, 0 );
	UpdateUi(&event);
}

TabletStateWidget::~TabletStateWidget()
{
}

void TabletStateWidget::LogMouseEvent( QMouseEvent * e )
{
    QString logText;
    logText += QString("Mouse: %1;").arg( MouseEventTypeToString( e ) );
    logText += QString(" buttons: %1;").arg( MouseEventButtonsToString( e ) );
    logText += QString(" pos: ( %1, %2 );").arg( e->x() ).arg( e->y() );

    ui.logWidget->appendPlainText( logText );
}

void TabletStateWidget::LogTabletEvent( QTabletEvent * e )
{
    QString logText;
    logText += QString("Tablet: %1;").arg( TabletEventTypeToString( e ) );
    logText += QString(" buttons: %1;").arg( TabletEventButtonsToString( e ) );
    logText += QString(" pos: ( %1, %2 );").arg( e->x() ).arg( e->y() );
    logText += QString(" presure: %1").arg( e->pressure() );
    ui.logWidget->appendPlainText( logText );

    UpdateUi( e );
}

void TabletStateWidget::LogEnterEvent()
{
    QString logText = "Event: Enter;";
    ui.logWidget->appendPlainText( logText );
}

void TabletStateWidget::LogLeaveEvent()
{
    QString logText = "Event: Leave;";
    ui.logWidget->appendPlainText( logText );
}

void TabletStateWidget::on_clearLogButton_clicked()
{
    ui.logWidget->clear();
}

void TabletStateWidget::UpdateUi( QTabletEvent * e )
{
    ui.moveEventsCheckBox->blockSignals( true );
    ui.moveEventsCheckBox->setChecked( ui.openGLWidget->GetLogMoveEvents() );
    ui.moveEventsCheckBox->blockSignals( false );

    ui.mouseTrackingCheckBox->blockSignals( true );
    ui.mouseTrackingCheckBox->setChecked( ui.openGLWidget->hasMouseTracking() );
    ui.mouseTrackingCheckBox->blockSignals( false );

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

QString TabletStateWidget::MouseEventTypeToString( QMouseEvent * e )
{
    QString s = "Unknown";
    if( e->type() == QEvent::MouseButtonPress )
        s = "Pressed";
    else if( e->type() == QEvent::MouseButtonRelease )
        s = "Released";
    else if( e->type() == QEvent::MouseMove )
        s = "Move";
    else if( e->type() == QEvent::MouseButtonDblClick )
        s = "Double-click";
    return s;
}

QString TabletStateWidget::TabletEventTypeToString( QTabletEvent * e )
{
    QString s = "Unknown";
    if( e->type() == QEvent::TabletPress )
    {
        s = "Pressed";
    }
    else if( e->type() == QEvent::TabletRelease )
    {
        s = "Released";
    }
    else if( e->type() == QEvent::TabletMove )
    {
        s = "Move";
    }
    else if( e->type() == QEvent::TabletEnterProximity )
    {
        s = "In Proximity";
    }
    else if( e->type() == QEvent::TabletLeaveProximity )
    {
        s = "Out of Proximity";
    }
    return s;
}

QString TabletStateWidget::MouseEventButtonsToString( QMouseEvent * e )
{
    QString s = "";
    if( e->buttons() & Qt::LeftButton )
        s += "Left ";
    if( e->buttons() & Qt::RightButton )
        s += "Right";
    if( e->buttons() & Qt::MiddleButton )
        s += "Mid";
    if( s.isEmpty() )
        s += "Unknown";
    return s;
}

QString TabletStateWidget::TabletEventButtonsToString( QTabletEvent * e )
{
    QString s = "";
    if( e->buttons() & Qt::LeftButton )
        s += "Left ";
    if( e->buttons() & Qt::RightButton )
        s += "Right";
    if( e->buttons() & Qt::MiddleButton )
        s += "Mid";
    if( s.isEmpty() )
        s += "Unknown";
    return s;
}

void TabletStateWidget::on_moveEventsCheckBox_toggled(bool checked)
{
    ui.openGLWidget->SetLogMoveEvents( checked );
}

void TabletStateWidget::on_mouseTrackingCheckBox_toggled(bool checked)
{
    ui.openGLWidget->setMouseTracking( checked );
}
