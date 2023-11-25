#include "TabletStateWidget.h"
#include "TabletStateTestingArea.h"
#include "drwDrawingWidget.h"

TabletStateWidget::TabletStateWidget( QWidget * parent ) : QWidget( parent )
{
    m_drawingWidget = 0;
	ui.setupUi(this);
    ui.openGLWidget->SetTabletStateWidget( this );
    QPointingDevice* device = new QPointingDevice();
    QTabletEvent event( QEvent::TabletEnterProximity, device, QPointF(0, 0), QPointF(0, 0), 0, 0, 0, 0, 0, 0, 
        Qt::KeyboardModifier::NoModifier, Qt::NoButton, Qt::NoButton);
	UpdateUi(&event);
}

TabletStateWidget::~TabletStateWidget()
{
}

void TabletStateWidget::SetDrawingWiget( drwDrawingWidget * w )
{
    m_drawingWidget = w;
    QPointingDevice* device = new QPointingDevice();
    QTabletEvent event(QEvent::TabletEnterProximity, device, QPointF(0, 0), QPointF(0, 0), 0, 0, 0, 0, 0, 0,
        Qt::KeyboardModifier::NoModifier, Qt::NoButton, Qt::NoButton);
    UpdateUi(&event);
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
    logText += QString("Tablet: %1 (id:%2);").arg( TabletEventTypeToString( e ) ).arg(e->uniqueId());
    logText += QString(" ptype: %1;").arg( TabletEventToPointerType( e ) );
    logText += QString(" dtype: %1").arg( TabletEventToDeviceType( e ) );
    logText += QString(" but: %1;").arg( TabletEventButtonsToString( e ) );
    logText += QString(" pos: ( %1, %2 );").arg( e->x() ).arg( e->y() );
    logText += QString(" pres: %1").arg( e->pressure() );
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

    if( m_drawingWidget )
    {
        ui.muteMouseCheckBox->blockSignals( true );
        ui.muteMouseCheckBox->setChecked( m_drawingWidget->IsMutingMouse() );
        ui.muteMouseCheckBox->blockSignals( false );
    }

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
        case QPointingDevice::PointerType::Unknown:
            pType = "QTabletEvent::UnknownPointer";
            break;
        case QPointingDevice::PointerType::Pen:
            pType = "QTabletEvent::Pen";
            break;
        case QPointingDevice::PointerType::Cursor:
            pType = "QTabletEvent::Cursor";
            break;
        case QPointingDevice::PointerType::Eraser:
            pType = "QTabletEvent::Eraser";
            break;
        default:
            pType = "NONE";
            break;
    }
    text += QString("Pointer type:         ") + pType + "\n";
    QString tabletDevice;
    switch ( e->device()->type() )
    {
        case QInputDevice::DeviceType::Unknown:
            tabletDevice = "QTabletEvent::NoDevice";
            break;
        case QInputDevice::DeviceType::Puck:
            tabletDevice = "QTabletEvent::Puck";
            break;
        case QInputDevice::DeviceType::Stylus:
            tabletDevice = "QTabletEvent::Stylus";
            break;
        case QInputDevice::DeviceType::Airbrush:
            tabletDevice = "QTabletEvent::Airbrush";
            break;
        case QInputDevice::DeviceType::Mouse:
            tabletDevice = "QTabletEvent::FourDMouse";
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

QString TabletStateWidget::TabletEventToPointerType( QTabletEvent * e )
{
    QString s = "";
    if( e->pointerType() == QPointingDevice::PointerType::Unknown)
        s = "Unknown";
    else if( e->pointerType() == QPointingDevice::PointerType::Pen )
        s = "Pen";
    else if( e->pointerType() == QPointingDevice::PointerType::Cursor )
        s = "Cursor";
    else if( e->pointerType() == QPointingDevice::PointerType::Eraser )
        s = "Eraser";
    return s;
}

QString TabletStateWidget::TabletEventToDeviceType( QTabletEvent * e )
{
    QString s = "";
    if( e->device()->type() == QInputDevice::DeviceType::Unknown)
        s = "NoDevice";
    else if( e->device()->type() == QInputDevice::DeviceType::Puck )
        s = "Puck";
    else if( e->device()->type() == QInputDevice::DeviceType::Stylus )
        s = "Stylus";
    else if( e->device()->type() == QInputDevice::DeviceType::Airbrush )
        s = "Airbrush";
    else if( e->device()->type() == QInputDevice::DeviceType::Mouse )
        s = "FourDMouse";
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

void TabletStateWidget::on_muteMouseCheckBox_toggled(bool checked)
{
    m_drawingWidget->SetMuteMouse( checked );
}

void TabletStateWidget::on_nativeEventCheckBox_toggled(bool checked)
{

}
