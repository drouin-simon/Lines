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
    void Reset();

    // Stream commands ( network and loading )
    void BeginCommandStream();
    void EndCommandStream();

    // Modify line tool params
    void SetLineErase( bool erase );
    void SetLineBaseWidth( double w );
    void SetLineColor( Vec4 color );
    void SetLinePressureWidth( bool pw );
    void SetLinePressureOpacity( bool po );
    void SetLineFill( bool f );
    void SetLinePersistence( int p );

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
    bool NeedsRender();
    void Render();
    void NotifyNeedRender();  // only for drwGlobalLineParams. todo: We could inside LinesCore

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

public slots:

    // Command dispatch
    void IncomingNetCommand( drwCommand::s_ptr );
    void IncomingLocalCommand( drwCommand::s_ptr );
    void IncomingDbCommand( drwCommand::s_ptr );

protected slots:

    void FrameChangedSlot();
    void NumberOfFramesChangedSlot();

signals:

    void DisplaySettingsModified();
    void PlaybackSettingsChangedSignal();
    void PlaybackStartStop( bool );

private:

    drwToolbox * AddUser( int commandUserId );
    void ClearAllToolboxesButLocal();

    // Call before and after changing the params of a tool
    void BeginChangeToolParams();
    void EndChangeToolParams();

    // Attributes for playback
    QTime m_time;
    int   m_frameInterval;  // number of miliseconds between frames
    bool  m_isPlaying;
    int	  m_lastFrameWantedTime;

    // Container to cache state commands until an effective command comes
    typedef QList< drwCommand::s_ptr > CommandContainer;
    CommandContainer m_cachedStateCommands;

    // List of local commands that need to be processed. This needs to be
    // protected by a mutex because it may be accessed from both render thread
    // and main thread.
    void PushLocalCommand( drwCommand::s_ptr com );
    void ExecuteLocalCommands();
    int m_stackedCurrentFrame;
    QMutex m_localCommandsMutex;
    CommandContainer m_localCommandsToProcess;

    // List of Network commands that need to be processed. This needs to be
    // protected by a mutex because it may be accessed from both render thread
    // and main thread.
    void PushNetCommand( drwCommand::s_ptr com );
    void ExecuteNetCommands();
    void ExecuteOneNetCommand( drwCommand::s_ptr com );
    QMutex m_netCommandsMutex;
    CommandContainer m_netCommandsToProcess;

     // Attribute to used to dispatch external (Network) commands
    static const int m_localToolboxId;
    int m_lastUsedUserId;
    typedef QMap< int, drwToolbox* > ToolboxContainer;
    ToolboxContainer m_toolboxes;

    // For loading and importing animation
    bool m_commandsPassThrough;

    Scene                * m_scene;
    drwGLRenderer        * m_renderer;
    drwDrawingSurface    * m_drawingSurface;
    drwToolbox           * m_localToolbox;
    drwCommandDatabase	 * m_commandDb;
    drwRemoteCommandIO   * m_remoteIO;

};

#endif
