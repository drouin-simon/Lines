#ifndef __LinesApp_h_
#define __LinesApp_h_

#include <QObject>
#include <SVL.h>

#include "drwLineTool.h"

class LinesCore;

class LinesApp : public QObject
{

    Q_OBJECT

public:

    LinesApp( LinesCore * lc );
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

signals:

    void LineParamsModified();
    void DisplayParamsModified();

private slots:

    void LineParamsModifiedSlot();
    void DisplayParamsModifiedSlot();

private:

    drwLineTool * GetLineTool();

    LinesCore * m_lines;

};

#endif
