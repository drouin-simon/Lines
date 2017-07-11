#ifndef __LinesCore_h_
#define __LinesCore_h_

#include <QObject>
#include <QTime>
#include <QMap>
#include <QMutex>
#include "drwCommand.h"

class drwDrawingSurface;
class Scene;
class drwToolbox;
class drwLineTool;
class drwCommandDatabase;
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
    void NewAnimation();

    // Setting properties
    void SetDrawingSurface( drwDrawingSurface * surface );
    void SetBackgroundColor( double r, double g, double b, double a );
    void SetRenderSize( int w, int h );
    void SetShowCursor( bool show );
    bool GetOnionSkinEnabled();
    void SetOnionSkinEnabled( bool e );
    int GetOnionSkinBefore();
    void SetOnionSkinBefore( int n );
    int GetOnionSkinAfter();
    void SetOnionSkinAfter( int n );
    void ToggleInOnionFrame();
    void ToggleOutOnionFrame();

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
    bool IsPlaying();
    void StartPlaying();
    void StopPlaying();
    void PlayPause();
    void Tick();

    // For Network Interface
    void SetRemoteIO( drwRemoteCommandIO * io );
    int RequestNewUserId();
    int GetLocalUserId();
    int GetNumberOfDbCommands();
    drwCommand::s_ptr GetDbCommand( int index );
    void LockDb( bool l );
    void IncomingNetCommand( drwCommand::s_ptr );

public slots:

    // Command dispatch
    void IncomingLocalCommand( drwCommand::s_ptr );
    void IncomingDbCommand( drwCommand::s_ptr );

protected slots:

    void FrameChangedSlot();
    void NumberOfFramesChangedSlot();
    void ProcessPendingNetCommandsSlot();

signals:

    void DisplaySettingsModified();
    void PlaybackSettingsChangedSignal();
    void PlaybackStartStop( bool );
    void NetCommandAvailable();

private:

    drwToolbox * AddUser( int commandUserId );
    void ClearAnimation();
    void ClearAllToolboxesButLocal();

    // Attributes for playback
    QTime m_time;
    int   m_frameInterval;  // number of miliseconds between frames
    bool  m_isPlaying;
    int	  m_lastFrameWantedTime;

    bool m_onionSkinEnabled;

    // Container to cache state commands until an effective command comes
    typedef QList< drwCommand::s_ptr > CommandContainer;
    CommandContainer m_cachedStateCommands;

    // List of Network commands that need to be processed. This needs to be
    // protected by a mutex because it may be accessed from different threads
    void ExecuteNetCommands();
    void ExecuteOneNetCommand( drwCommand::s_ptr com );
    QMutex m_netCommandsMutex;
    CommandContainer m_netCommandsToProcess;

     // Attribute to used to dispatch external (Network) commands
    static const int m_localToolboxId;
    int m_lastUsedUserId;
    typedef QMap< int, drwToolbox* > ToolboxContainer;
    ToolboxContainer m_toolboxes;

    Scene                * m_scene;
    drwGLRenderer        * m_renderer;
    drwDrawingSurface    * m_drawingSurface;
    drwToolbox           * m_localToolbox;
    drwCommandDatabase	 * m_commandDb;
    drwRemoteCommandIO   * m_remoteIO;

};

#endif
