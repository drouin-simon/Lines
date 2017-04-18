#ifndef __PlaybackControler_h_
#define __PlaybackControler_h_

#include <QObject>
#include <QTime>

class Scene;
class drwToolbox;

class PlaybackControler : public QObject
{
	Q_OBJECT
	
public:
	
    PlaybackControler( Scene * scene );
	~PlaybackControler();
	
    void SetToolbox( drwToolbox * t );

    void SetPlaying( bool play );
    void StartPlaying();
    void StopPlaying();
    void PlayPause();
	bool IsPlaying() { return isPlaying; }
	
	void SetCurrentFrame( int frame );
    void NotifyFrameChanged();
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
	
signals:
	
	void ModifiedSignal();
	void StartStop( bool isStart );
	
protected:
	
	Scene			* m_scene;
    drwToolbox      * m_toolbox;
	QTime			time;
	int				frameInterval;  // number of miliseconds between frames
	bool			isPlaying;
	bool			loop;
	int				m_lastFrameWantedTime;
};


#endif
