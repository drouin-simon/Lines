#ifndef __drwWidgetObserver_h_
#define __drwWidgetObserver_h_

#include <QObject>
#include "drwCommand.h"

class Scene;
class drwDrawingSurface;
class QMouseEvent;
class QTabletEvent;
class QDropEvent;
class QSettings;

class drwWidgetObserver : public QObject
{
	
	Q_OBJECT

public:
	
	drwWidgetObserver( Scene * scene, QObject * parent = 0 ) : QObject(parent) { CurrentScene = scene; CurrentFrame = 0; }
	virtual ~drwWidgetObserver() {};
    
    // Read and Write settings
    virtual void ReadSettings( QSettings & s ) {}
    virtual void WriteSettings( QSettings & s ) {}
	
	virtual void SetCurrentFrame( int frame ) { CurrentFrame = frame; }
    virtual void Reset() {}
	
	virtual void ExecuteCommand( drwCommand::s_ptr command ) {}
	
signals:
	
	void CommandExecuted( drwCommand::s_ptr command );
	void StartInteraction();
	void EndInteraction();
	
protected:
	
	Scene * CurrentScene;
	int CurrentFrame;
};

#endif
