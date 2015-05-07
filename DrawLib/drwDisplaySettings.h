#ifndef __drwDisplaySettings_h_
#define __drwDisplaySettings_h_

#include <QObject>

class drwDisplaySettings : public QObject
{
	
	Q_OBJECT
	
public:
	
	drwDisplaySettings( QObject * parent = 0 );
	~drwDisplaySettings();
	
	int GetOnionSkinBefore() { return m_onionSkinFramesBefore; }
	void SetOnionSkinBefore( int );
	int GetOnionSkinAfter() { return m_onionSkinFramesAfter; }
	void SetOnionSkinAfter( int );
	bool GetInhibitOnionSkin() { return m_inhibitOnionSkin; }
	void SetInhibitOnionSkin( bool isOn );
	bool GetShowCameraFrame() { return m_showCameraFrame; }
	void SetShowCameraFrame( bool );
	
signals:
	
	void ModifiedSignal();
	
private:
	
	int m_onionSkinFramesBefore;
	int m_onionSkinFramesAfter;
	bool m_inhibitOnionSkin;
	bool m_showCameraFrame;
	
};

#endif