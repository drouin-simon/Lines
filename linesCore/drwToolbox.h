#ifndef __drwToolbox_h_
#define __drwToolbox_h_

#include <QObject>
#include <vector>
#include "drwCommand.h"

class Scene;
class LinesCore;
class drwGLRenderer;
class drwTool;
class QSettings;

class drwToolbox : public QObject
{
	
Q_OBJECT
	
public:
	
    drwToolbox( Scene * scene, LinesCore * lc, bool local );
	virtual ~drwToolbox();

    bool IsLocal() { return m_isLocal; }
	
    void SetRenderer( drwGLRenderer * r );
    drwGLRenderer * GetRenderer() { return m_renderer; }

    void AddTool( drwTool * tool );
    drwTool * GetTool( int index );
    
    // Read and Write settings
    void ReadSettings( QSettings & s );
    void WriteSettings( QSettings & s );

    void StartPlaying();
    bool IsPlaying();
    void OnStartPlaying();
    void StopPlaying();
    void OnStopPlaying();
    void SetCurrentFrame( int frame );
    int GetCurrentFrame() { return m_currentFrame; }
    void GotoNextFrame();
    void GotoPrevFrame();
    int GetNumberOfFrames();

    void EmitStateCommand();
    void PostAnimationLoad();
	
	void blockSignals( bool block );
	
	void ExecuteCommand( drwCommand::s_ptr );
    void SetShowCursor( bool show );
	
private slots:
	
	void NotifyCommandExecuted( drwCommand::s_ptr command );

signals:

    void CommandExecuted( drwCommand::s_ptr command );
    void FrameChanged();
	
protected:
	
    bool m_isLocal;
    int m_currentFrame;
    int m_currentTool;
    std::vector<drwTool*> Tools;
    Scene * m_scene;
    LinesCore * m_lines;
    drwGLRenderer * m_renderer;
};

#endif
