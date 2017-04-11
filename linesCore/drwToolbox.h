#ifndef __drwToolbox_h_
#define __drwToolbox_h_

#include <QObject>
#include <vector>
#include "drwCommand.h"

class Scene;
class PlaybackControler;
class drwGLRenderer;
class drwTool;
class QSettings;

class drwToolbox : public QObject
{
	
Q_OBJECT
	
public:
	
    drwToolbox( Scene * scene, PlaybackControler * c );
	virtual ~drwToolbox();
	
    void SetRenderer( drwGLRenderer * r );
    drwGLRenderer * GetRenderer() { return m_renderer; }

    void AddTool( drwTool * tool );
    drwTool * GetTool( int index );
    
    // Read and Write settings
    void ReadSettings( QSettings & s );
    void WriteSettings( QSettings & s );

    void StartPlaying();
    void OnStartPlaying();
    void StopPlaying();
    void OnStopPlaying();
    void SetCurrentFrame( int frame );
    int GetCurrentFrame() { return m_currentFrame; }
    void GotoNextFrame();
    void GotoPrevFrame();
    int GetNumberOfFrames();

    void Reset();
	
	void blockSignals( bool block );
	
	void ExecuteCommand( drwCommand::s_ptr );
	
private slots:
	
	void NotifyCommandExecuted( drwCommand::s_ptr command );

signals:

    void CommandExecuted( drwCommand::s_ptr command );
	
protected:
	
    int m_currentFrame;
    int m_currentTool;
    std::vector<drwTool*> Tools;
    Scene * m_scene;
    PlaybackControler * m_controller;
    drwGLRenderer * m_renderer;
};

#endif
