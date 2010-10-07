#ifndef __drwWidgetObserver_h_
#define __drwWidgetObserver_h_

#include <QObject>
#include "drwCommand.h"

class Scene;
class drwDrawingWidget;
class QMouseEvent;
class QTabletEvent;
class QDropEvent;

class drwWidgetObserver : public QObject
{
	
	Q_OBJECT

public:
	
	drwWidgetObserver( Scene * scene, QObject * parent = 0 ) : QObject(parent) { CurrentScene = scene; CurrentFrame = 0; Persistence = 1; }
	virtual ~drwWidgetObserver() {};
	
	virtual void MousePressEvent( drwDrawingWidget * w, QMouseEvent * e ) {}
	virtual void MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e ) {}
	virtual void MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e ) {}
	virtual void TabletEvent( drwDrawingWidget * w, QTabletEvent * e ) {}
	virtual void DropEvent( drwDrawingWidget * w, QDropEvent * e ) {}
	virtual void EnterEvent( drwDrawingWidget * w, QEvent * e ) {}
	virtual void LeaveEvent( drwDrawingWidget * w, QEvent * e ) {}
	
	virtual void SetCurrentFrame( int frame ) { CurrentFrame = frame; }
	virtual void Reset() {}
	virtual void ToggleBrushEraser() {}
	
public slots:
	
	virtual void ExecuteCommand( drwCommand::s_ptr command ) {}
	virtual void SetPersistence( int nbFrames ) { Persistence = nbFrames; }
	
signals:
	
	void CommandExecuted( drwCommand::s_ptr command );
	void StartInteraction();
	void EndInteraction();
	
protected:
	
	Scene * CurrentScene;
	int CurrentFrame;
	int Persistence;
	
};

#endif
