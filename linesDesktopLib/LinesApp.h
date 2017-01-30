#ifndef __LinesApp_h_
#define __LinesApp_h_

#include <QObject>
#include "drwEditionState.h"
#include <SVL.h>

class drwDrawingWidget;
class drwToolbox;
class drwLineTool;
class drwCursor;

class LinesApp : public QObject
{

    Q_OBJECT

public:

    LinesApp( drwEditionState * edState, drwToolbox * toolbox );
    ~LinesApp();

    void SetDrawingWidget( drwDrawingWidget * w );
    void SetCursor( drwCursor * cursor ) { m_cursor = cursor; }

    // Line brush
    bool IsSmallBrush();
    void SetSmallBrush();
    bool IsBigBrush();
    void SetBigBrush();
    void ToggleBigSmallBrush();
    void IncreaseBrushSize();
    void DecreaseBrushSize();
    void SetLineWidth( double w );
    double GetLineWidth();
    void SetLineColor( Vec4 & color );
    Vec4 GetLineColor();
    void SetErasing();
    bool IsErasing();
    void ToggleErasing();

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

    double m_backupBrushWidth;
    double m_backupBrushOpacity;

    drwEditionState * m_editionState;
    drwToolbox * m_localToolbox;
    drwDrawingWidget * m_drawingWidget;
    drwCursor * m_cursor;

};

#endif
