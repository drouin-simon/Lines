#include "TabletStateWidget.h"
#include "TabletStateTestingArea.h"
#include "drwDrawingWidget.h"
#include <QPointingDevice>

static QTabletEvent createDefaultTabletEvent()
{
    static const QPointingDevice defaultDevice( "default", 0, QInputDevice::DeviceType::Unknown,
        QPointingDevice::PointerType::Unknown, QInputDevice::Capability::None, 0, 0 );
    return QTabletEvent( QEvent::TabletEnterProximity, &defaultDevice, QPointF(0,0), QPointF(0,0),
        0, 0, 0, 0, 0, 0, Qt::NoModifier, Qt::NoButton, Qt::NoButton );
}

TabletStateWidget::TabletStateWidget( QWidget * parent ) : QWidget( parent )
{
    m_drawingWidget = 0;
	ui.setupUi(this);
    ui.openGLWidget->SetTabletStateWidget( this );
    QTabletEvent event = createDefaultTabletEvent();
	UpdateUi(&event);
}

TabletStateWidget::~TabletStateWidget()
{
}

void TabletStateWidget::SetDrawingWiget( drwDrawingWidget * w )
{
    m_drawingWidget = w;
    QTabletEvent event = createDefaultTabletEvent();
    UpdateUi(&event);
}

void TabletStateWidget::LogMouseEvent( QMouseEvent * e )
{
    QString logText;
    logText += QString("Mouse: %1;").arg( MouseEventTypeToString( e ) );
    logText += QString(" buttons: %1;").arg( MouseEventButtonsToString( e ) );
    logText += QString(" pos: ( %1, %2 );").arg( e->position().x() ).arg( e->position().y() );

    ui.logWidget->appendPlainText( logText );
}

void TabletStateWidget::LogTabletEvent( QTabletEvent * e )
{
    QString logText;
    logText += QString("Tablet: %1 (id:%2);").arg( TabletEventTypeToString( e ) ).arg(e->pointingDevice()->uniqueId().numericId());
    logText += QString(" ptype: %1;").arg( TabletEventToPointerType( e ) );
    logText += QString(" dtype: %1").arg( TabletEventToDeviceType( e ) );
    logText += QString(" but: %1;").arg( TabletEventButtonsToString( e ) );
    logText += QString(" pos: ( %1, %2 );").arg( e->position().x() ).arg( e->position().y() );
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
    switch ( e->pointingDevice()->pointerType() )
    {
        case QPointingDevice::PointerType::Unknown:
            pType = "Unknown";
            break;
        case QPointingDevice::PointerType::Generic:
            pType = "Generic";
            break;
        case QPointingDevice::PointerType::Finger:
            pType = "Finger";
            break;
        case QPointingDevice::PointerType::Pen:
            pType = "Pen";
            break;
        case QPointingDevice::PointerType::Eraser:
            pType = "Eraser";
            break;
        case QPointingDevice::PointerType::Cursor:
            pType = "Cursor";
            break;
        default:
            pType = "NONE";
            break;
    }
    text += QString("Pointer type:         ") + pType + "\n";
    QString tabletDevice;
    switch ( e->pointingDevice()->type() )
    {
        case QInputDevice::DeviceType::Unknown:
            tabletDevice = "Unknown";
            break;
        case QInputDevice::DeviceType::Mouse:
            tabletDevice = "Mouse";
            break;
        case QInputDevice::DeviceType::TouchScreen:
            tabletDevice = "TouchScreen";
            break;
        case QInputDevice::DeviceType::TouchPad:
            tabletDevice = "TouchPad";
            break;
        case QInputDevice::DeviceType::Puck:
            tabletDevice = "Puck";
            break;
        case QInputDevice::DeviceType::Stylus:
            tabletDevice = "Stylus";
            break;
        case QInputDevice::DeviceType::Airbrush:
            tabletDevice = "Airbrush";
            break;
        default:
            tabletDevice = "NONE";
            break;
    }
    text += QString("Tablet device:        ") + tabletDevice + "\n";
    text += QString("Position:             ( %1, %2, %3 )\n" ).arg( e->position().x()).arg( e->position().y() ).arg( e->z() );
    text += QString("Global Pos:           ( %1, %2 )\n" ).arg( e->globalPosition().x() ).arg( e->globalPosition().y() );
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
    auto pt = e->pointingDevice()->pointerType();
    if( pt == QPointingDevice::PointerType::Unknown )
        s = "Unknown";
    else if( pt == QPointingDevice::PointerType::Pen )
        s = "Pen";
    else if( pt == QPointingDevice::PointerType::Cursor )
        s = "Cursor";
    else if( pt == QPointingDevice::PointerType::Eraser )
        s = "Eraser";
    else if( pt == QPointingDevice::PointerType::Finger )
        s = "Finger";
    else if( pt == QPointingDevice::PointerType::Generic )
        s = "Generic";
    return s;
}

QString TabletStateWidget::TabletEventToDeviceType( QTabletEvent * e )
{
    QString s = "";
    auto dt = e->pointingDevice()->type();
    if( dt == QInputDevice::DeviceType::Unknown )
        s = "Unknown";
    else if( dt == QInputDevice::DeviceType::Puck )
        s = "Puck";
    else if( dt == QInputDevice::DeviceType::Stylus )
        s = "Stylus";
    else if( dt == QInputDevice::DeviceType::Airbrush )
        s = "Airbrush";
    else if( dt == QInputDevice::DeviceType::Mouse )
        s = "Mouse";
    else if( dt == QInputDevice::DeviceType::TouchScreen )
        s = "TouchScreen";
    else if( dt == QInputDevice::DeviceType::TouchPad )
        s = "TouchPad";
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
