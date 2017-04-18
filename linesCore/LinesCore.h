#ifndef __LinesCore_h_
#define __LinesCore_h_

#include <QObject>
#include "drwCommand.h"

class drwDrawingSurface;
class Scene;
class PlaybackControler;
class drwToolbox;
class drwLineTool;
class drwCommandDatabase;
class drwCommandDispatcher;
class drwGLRenderer;
class drwRemoteCommandIO;
class QSettings;

class LinesCore : public QObject
{
    Q_OBJECT

public:

    LinesCore();
    ~LinesCore();

    drwLineTool * GetLineTool();
    Scene * GetScene() { return m_scene; }  // needed by exporter. todo : fix this in the future

    // Animation IO
    void LoadAnimation( const char * filename );
    void SaveAnimation( const char * filename );
    bool IsAnimationModified();
    void Reset();

    // Setting properties
    void SetDrawingSurface( drwDrawingSurface * surface );
    void SetBackgroundColor( double r, double g, double b, double a );
    void SetRenderSize( int w, int h );
    void SetShowCursor( bool show );
    int GetOnionSkinBefore();
    void SetOnionSkinBefore( int n );
    int GetOnionSkinAfter();
    void SetOnionSkinAfter( int n );

    // Read/Write settings
    void ReadSettings( QSettings & s );
    void WriteSettings( QSettings & s );

    // Rendering
    void Render();
    void NotifyNeedRender();  // only for drwGlobalLineParams. todo: We could inside LinesCore
    void EnableRendering( bool enable );  // needed to block rendering when receiving an animation. todo: use same mechanism for loading and receiving

    // Local drawing
    void MouseEvent( drwMouseCommand::MouseCommandType commandType, double xPix, double yPix, double pressure = 1.0,
                     int xTilt = 0, int yTilt = 0, double rotation = 0.0, double tangentialPressure = 0.0 );
    void MouseEventWorld( drwMouseCommand::MouseCommandType type, double xWorld, double yWorld, double pressure );

    // Playback
    int GetNumberOfFrames();
    void SetNumberOfFrames( int nb );
    int GetCurrentFrame();
    void SetCurrentFrame( int frame );
    void NextFrame();
    void PrevFrame();
    void GotoStart();
    void GotoEnd();
    void SetFrameInterval( int intervalms );
    bool IsLooping();
    void SetLooping( bool loop );
    bool IsPlaying();
    void PlayPause();
    void Tick();

    // For Network Interface
    void SetRemoteIO( drwRemoteCommandIO * io );
    int RequestNewUserId();
    int GetLocalUserId();
    void IncomingNetCommand( drwCommand::s_ptr com );
    int GetNumberOfDbCommands();
    drwCommand::s_ptr GetDbCommand( int index );
    void LockDb( bool l );

protected slots:

    void PlaybackStartStop( bool isStarting );
    void PlaybackSettingsChangedSlot();

signals:

    void DisplaySettingsModified();
    void PlaybackSettingsChangedSignal();

private:

    Scene		 * m_scene;
    drwGLRenderer        * m_renderer;
    drwDrawingSurface    * m_drawingSurface;
    PlaybackControler	 * m_controler;
    drwToolbox		 * m_localToolbox;
    drwCommandDatabase	 * m_commandDb;
    drwCommandDispatcher * m_commandDispatcher;

};

#endif
