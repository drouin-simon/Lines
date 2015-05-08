#ifndef __PlaybackControler_h_
#define __PlaybackControler_h_

#include <QObject>
#include <QTime>
#include "drwGlobal.h"
#include "drwEditionState.h"

class Scene;
class drwEditionState;

class PlaybackControler : public QObject
{
	Q_OBJECT
	
public:
	
	PlaybackControler( Scene * scene, QObject * parent = 0 );
	~PlaybackControler();
	
	drwEditionState * GetEditionState() { return m_editionState; }
	
	void PlayPause( bool manual = true );
	bool IsPlaying() { return isPlaying; }
	
	void SetCurrentFrame( int frame );
	int GetCurrentFrame();
    int GetNextFrame();
	int GetNumberOfFrames();
	
	void NextFrame();
	void PrevFrame();
	void GotoStart();
	void GotoEnd();
	
	void SetLooping( bool looping );
	bool GetLooping() { return loop; }
	void ToggleLooping();
	
	// Forces to reconsider the current time
	// This is used to advance frames not only on timer events, but on
	// all user interactions.
	bool Tick();
	
public slots:
	
	void NumberOfFramesChangedSlot( int nbFrames );
	void SetFrameInterval( int msecPerFrame );
	void StartInteraction();
	void EndInteraction();
	
signals:
	
	void ModifiedSignal();
	void FrameChanged( int newFrame );
	void StartStop( bool isStart );
	
protected:
	
	Scene			* m_scene;
	drwEditionState * m_editionState;
    drwFrameChangeMode m_backupFrameChangeMode;
	QTime			time;
	int				frameInterval;  // number of miliseconds between frames
	bool			isPlaying;
	bool			loop;
	int				interactionStartFrame;
	bool			insertFrameMode;
	int				m_lastFrameWantedTime;
};


#endif
