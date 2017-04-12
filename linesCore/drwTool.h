#ifndef __drwTool_h_
#define __drwTool_h_

#include <QObject>
#include "drwCommand.h"

class Scene;
class QSettings;
class drwToolbox;

class drwTool : public QObject
{
	
	Q_OBJECT

public:
	
    drwTool( Scene * scene, drwToolbox * toolbox ) : m_scene( scene ), m_toolbox( toolbox ) {}
    virtual ~drwTool() {};

    virtual void OnStartPlaying() {}
    virtual void OnStopPlaying() {}
    
    // Read and Write settings
    virtual void ReadSettings( QSettings & s ) {}
    virtual void WriteSettings( QSettings & s ) {}
	
    virtual void NotifyFrameChanged( int frame ) {}
    virtual void Reset() {}

    virtual void NotifyRendererChanged() {}
    virtual void SetShowCursor( bool show ) {}
	
	virtual void ExecuteCommand( drwCommand::s_ptr command ) {}
	
signals:
	
	void CommandExecuted( drwCommand::s_ptr command );
	
protected:
	
    Scene * m_scene;
    drwToolbox * m_toolbox;
};

#endif
