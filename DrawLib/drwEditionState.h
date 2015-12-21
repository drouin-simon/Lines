#ifndef __drwEditionState_h_
#define __drwEditionState_h_

#include <QObject>

class QSettings;

enum drwFrameChangeMode{ Manual, AfterIntervention, Play };

// This class stores all global state information that is needed by drawing tools
class drwEditionState : public QObject
{
	
	Q_OBJECT
	
public:
	
	drwEditionState( QObject * parent = 0 );
	~drwEditionState();
	
	int GetCurrentFrame() { return m_currentFrame; }
	void SetCurrentFrame(int);
	drwFrameChangeMode GetFrameChangeMode() { return m_frameChangeMode; }
	void SetFrameChangeMode( drwFrameChangeMode mode );
    void SetPersistence( int nbFrames );
    int GetPersistence() { return m_persistence; }
    bool IsPersistenceEnabled() { return m_frameChangeMode == Play; }
	void Reset();
    void ReadSettings( QSettings & s );
    void WriteSettings( QSettings & s );
	
signals:
	
	void CurrentFrameChanged(int);
	void ModifiedSignal();
	
private:
	
	int m_currentFrame;
	int m_currentLayer;
	drwFrameChangeMode m_frameChangeMode;
    int m_persistence;
};


#endif
