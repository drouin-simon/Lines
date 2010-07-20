#ifndef __drwEditionState_h_
#define __drwEditionState_h_

#include <QObject>

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
	int GetPersistence() { return m_persistence; }
	void SetPersistence(int);
	drwFrameChangeMode GetFrameChangeMode() { return m_frameChangeMode; }
	void SetFrameChangeMode( drwFrameChangeMode mode );
	void Reset();
	
signals:
	
	void CurrentFrameChanged(int);
	void ModifiedSignal();
	
private:
	
	int m_currentFrame;
	int m_currentLayer;
	int m_persistence;
	drwFrameChangeMode m_frameChangeMode;
};


#endif