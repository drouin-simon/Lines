#ifndef __drwTool_h_
#define __drwTool_h_

#include <QObject>
#include "drwCommand.h"
#include "drwToolbox.h"

class Scene;
class QSettings;

class drwTool : public QObject
{
	
	Q_OBJECT

public:
	
    drwTool( Scene * scene, drwToolbox * toolbox ) : m_scene( scene ), m_toolbox( toolbox ) {}
    virtual ~drwTool() {};

    virtual void OnStartPlaying() {}
    virtual void OnStopPlaying() {}

    bool IsLocal() { return m_toolbox->IsLocal(); }
    
    // Read and Write settings
    virtual void ReadSettings( QSettings & s ) {}
    virtual void WriteSettings( QSettings & s ) {}
	
    virtual void NotifyFrameChanged( int frame ) {}
    virtual void EmitStateCommand() {}
    virtual void PostAnimationLoad() {}

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
