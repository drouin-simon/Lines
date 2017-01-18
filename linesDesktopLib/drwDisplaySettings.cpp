#include "drwDisplaySettings.h"

drwDisplaySettings::drwDisplaySettings( QObject * parent )
: QObject( parent )
, m_onionSkinFramesBefore(1)
, m_onionSkinFramesAfter(0)
, m_inhibitOnionSkin(false)
, m_showCameraFrame(true)
{
}

drwDisplaySettings::~drwDisplaySettings()
{
}

void drwDisplaySettings::SetOnionSkinBefore( int value )
{
	m_onionSkinFramesBefore = value;
	emit ModifiedSignal();
}

void drwDisplaySettings::SetOnionSkinAfter( int value )
{
	m_onionSkinFramesAfter = value;
	emit ModifiedSignal();
}

void drwDisplaySettings::SetInhibitOnionSkin( bool isOn )
{
	m_inhibitOnionSkin = isOn;
	emit ModifiedSignal();
}

void drwDisplaySettings::SetShowCameraFrame( bool isOn )
{
	m_showCameraFrame = isOn;
	emit ModifiedSignal();
}
