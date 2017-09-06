#ifndef __LinesApp_h_
#define __LinesApp_h_

#include <QObject>
#include <SVL.h>

#include "drwLineTool.h"

class LinesCore;
class drwNetworkManager;

class LinesApp : public QObject
{

    Q_OBJECT

public:

    LinesApp( LinesCore * lc, drwNetworkManager * netManager );
    ~LinesApp();

    // Line brush
    bool IsBrush();
    void UseBrush();
    void IncreaseBrushSize();
    void DecreaseBrushSize();
    void SetLineWidth( double w );
    double GetLineWidth();
    void SetLineColor( Vec4 & color );
    Vec4 GetLineColor();
    void SetErasing();
    bool IsErasing();
    bool IsPressureWidthEnabled();
    bool IsUsingPressureWidth();
    void SetUsePressureWidth( bool use );
    bool IsPressureOpacityEnabled();
    bool IsUsingPressureOpacity();
    void SetUsePressureOpacity( bool use );
    bool IsFilling();
    void SetFill( bool fill );
    void SetPersistence( int p );
    int GetPersistence();

    // Control
    void GotoNextFrame();
    void GotoPreviousFrame();

    // Frame Change mode
    bool IsFrameChangeManual();
    void SetFrameChangeManual();
    bool IsFrameChangeJumpAfter();
    void SetFrameChangeJumpAfter();
    bool IsFrameChangePlay();
    void SetFrameChangePlay();
    drwFrameChangeMode GetFrameChangeMode();
    void SetFrameChangeMode( drwFrameChangeMode mode );

    // Onion Skin
    void ToggleOnionSkinEnabled();
    bool IsOnionSkinEnabled();
    void SetOnionSkinEnabled( bool enabled );
    void SetOnionSkinBefore( int nbFrames );
    int GetOnionSkinBefore();
    void SetOnionSkinAfter( int nbFrames );
    int GetOnionSkinAfter();
    void ToggleInOnionSkin();
    void ToggleOutOnionSkin();

    // Frame flipping mode
    bool IsFlippingModeEnabled();
    void SetFlippingModeEnabled( bool enabled );

    // Network
    bool IsSharing();
    bool IsConnected();
    QString GetServerName();
    bool IsMaster();

signals:

    void LineParamsModified();
    void DisplayParamsModified();
    void NetworkManagerStateChangedSignal();

private slots:

    void LineParamsModifiedSlot();
    void DisplayParamsModifiedSlot();
    void NetworkManagerStateChangedSlot();

private:

    drwLineTool * GetLineTool();

    LinesCore * m_lines;
    drwNetworkManager * m_netManager;

};

#endif
