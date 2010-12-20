#ifndef __drwToolbox_h_
#define __drwToolbox_h_

#include <QObject>
#include <vector>
#include "drwCommand.h"
#include "drwWidgetObserver.h"

class Scene;
class drwEditionState;

class drwToolbox : public drwWidgetObserver
{
	
Q_OBJECT
	
public:
	
	drwToolbox( Scene * scene, drwEditionState * editionState, QObject * parent );
	virtual ~drwToolbox();
	
	void AddTool( drwWidgetObserver * tool );
	
	// Implementation of drwWidgetObserver
	virtual void MousePressEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void TabletEvent( drwDrawingWidget * w, QTabletEvent * e );
	virtual void DropEvent( drwDrawingWidget * w, QDropEvent * e );
	virtual void EnterEvent( drwDrawingWidget * w, QEvent * e );
	virtual void LeaveEvent( drwDrawingWidget * w, QEvent * e );
	
	virtual void SetCurrentFrame( int frame );
	virtual void Reset();
	
	void blockSignals( bool block );
	
	void ToggleBrushEraser();
	
public slots:
	
	virtual void SetPersistence( int nbFrames );
	
private slots:
	
	void NotifyCommandExecuted( drwCommand::s_ptr command );
	void NotifyStartInteraction();
	void NotifyEndInteraction();
	
public slots:
	
	void ExecuteCommand( drwCommand::s_ptr );
	
protected:
	
	unsigned CurrentTool;
	std::vector<drwWidgetObserver*> Tools;
	
	drwEditionState * m_editionState;
	
};

#endif
