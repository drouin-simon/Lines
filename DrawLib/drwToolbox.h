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
	
	drwToolbox( Scene * scene, drwEditionState * editionState, QObject * parent = 0 );
	virtual ~drwToolbox();
	
	void AddTool( drwWidgetObserver * tool );
	drwWidgetObserver * GetTool( int index );
    
    // Read and Write settings
    void ReadSettings( QSettings & s );
    void WriteSettings( QSettings & s );
	
	// Implementation of drwWidgetObserver
	virtual void MousePressEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void TabletEvent( drwDrawingWidget * w, QTabletEvent * e );
	virtual void DropEvent( drwDrawingWidget * w, QDropEvent * e );
	
	virtual void SetCurrentFrame( int frame );
	virtual void Reset();
	
	void blockSignals( bool block );
	
	void ExecuteCommand( drwCommand::s_ptr );
	
private slots:
	
	void NotifyCommandExecuted( drwCommand::s_ptr command );
	void NotifyStartInteraction();
	void NotifyEndInteraction();
	
protected:
	
	unsigned CurrentTool;
	std::vector<drwWidgetObserver*> Tools;
	
	drwEditionState * m_editionState;
	
};

#endif
