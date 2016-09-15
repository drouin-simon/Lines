#ifndef __LinesApp_h_
#define __LinesApp_h_

#include <QObject>
#include "drwEditionState.h"
#include <SVL.h>

class drwDisplaySettings;
class drwToolbox;
class drwLineTool;

class LinesApp : public QObject
{

    Q_OBJECT

public:

    LinesApp( drwEditionState * edState, drwToolbox * toolbox, drwDisplaySettings * dispSettings );
    ~LinesApp();

    // Line brush
    bool IsSmallBrush();
    void SetSmallBrush();
    bool IsBigBrush();
    void SetBigBrush();
    void SetLineWidth( double w );
    double GetLineWidth();
    void SetLineColor( Vec4 & color );
    Vec4 GetLineColor();
    void SetErasing();
    bool IsErasing();

    // Frame Change mode
    bool IsFrameChangeManual();
    void SetFrameChangeManual();
    bool IsFrameChangeJumpAfter();
    void SetFrameChangeJumpAfter();
    drwFrameChangeMode GetFrameChangeMode();
    void SetFrameChangeMode( drwFrameChangeMode mode );

    // Onion Skin
    bool IsNoOnionSkin();
    void SetNoOnionSkin();
    bool IsOneOnionSkin();
    void SetOneOnionSkin();
    bool IsManyOnionSkin();
    void SetManyOnionSkin();
    void SetOnionSkinBefore( int nbFrames );
    int GetOnionSkinBefore();
    void SetOnionSkinAfter( int nbFrames );
    int GetOnionSkinAfter();

signals:

    void LineParamsModified();
    void EditParamsModified();
    void DisplayParamsModified();

private slots:

    void LineParamsModifiedSlot();
    void EditParamsModifiedSlot();
    void DisplayParamsModifiedSlot();

private:

    drwLineTool * GetLineTool();

    drwEditionState * m_editionState;
    drwToolbox * m_localToolbox;
    drwDisplaySettings * m_displaySettings;

};

#endif
