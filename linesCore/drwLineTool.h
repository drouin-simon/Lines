#ifndef __drwLineTool_h_
#define __drwLineTool_h_

#include "drwTool.h"
#include <SVL.h>
#include "macros.h"
#include <map>

class Node;
class LinePrimitive;
class drwToolbox;
class Circle;

enum PrimitiveType { TypeLine = 0, TypeWideLine, EndType };
enum drwFrameChangeMode{ Manual, AfterIntervention, Play };

class drwLineTool : public drwTool
{
	
	Q_OBJECT

public:
	
    drwLineTool( Scene * scene, drwToolbox * toolbox );
	virtual ~drwLineTool() {};
    
    // Read and Write settings
    void ReadSettings( QSettings & s );
    void WriteSettings( QSettings & s );

    // React to start/stop playback of animation
    virtual void OnStartPlaying();
    virtual void OnStopPlaying();

    // convenience functions used to automatically create lines
    void StartLine( double xWorld, double yWorld, double pressure = 1.0 );
    void AddPoint( double xWorld, double yWorld, double pressure = 1.0 );
    void EndLine( double xWorld, double yWorld, double pressure = 1.0 );
	
	virtual void ExecuteCommand( drwCommand::s_ptr command );
    virtual void NotifyFrameChanged( int frame );
	virtual void Reset();

    virtual void NotifyRendererChanged();
    virtual void SetShowCursor( bool show );

	// Get/Set brush properties - will all generate a line tool attribute command
	bool GetPressureWidth() { return m_pressureWidth; }
	void SetPressureWidth( bool w );
	bool GetPressureOpacity() { return m_pressureOpacity; }
	void SetPressureOpacity( bool o );
    bool IsPresureWidthEnabled();
    bool IsPresureOpacityEnabled();
	bool GetFill() { return m_fill; }
	void SetFill( bool f );
    bool GetErase() { return m_erase; }
    void SetErase( bool e );
	GetMacro( Color, Vec4 );
	void SetColor( Vec4 & c );
    drwFrameChangeMode GetFrameChangeMode() { return m_frameChangeMode; }
    void SetFrameChangeMode( drwFrameChangeMode mode );
    void SetPersistence( int nbFrames );
    int GetPersistence() { return m_persistence; }
    void SetPersistenceEnabled( bool enable );
    bool IsPersistenceEnabled() { return m_persistenceEnabled; }
    void SetBaseWidth( double bw );
    double GetBaseWidth();

signals:

    void CommandExecuted( drwCommand::s_ptr command );
	void ParametersChangedSignal();
	
protected:
	
	void ExecuteLineToolParamCommand( drwCommand::s_ptr command );
	void ExecuteMouseCommand( drwCommand::s_ptr command );
	void ParametersChanged();
    void CreateNewNodes( double x, double y, double pressure );
    Node * CreateNewNode( double x, double y, double pressure );
    void MarkSegmentModified( double x1, double y1, double x2, double y2 );
    void MarkPointModified( double x, double y );
    void MarkWholePrimitiveModified();
    void MarkOverlayModified();
    void UpdateCursor();
	
    double m_lastXWorld;
    double m_lastYWorld;
    double m_lastPressure;
	
    bool m_isDrawing;
    
    // prevents mouse commands to be generated while tablet is active
    // for drivers that send both mouse and tablet events to the program
    bool m_tabletHasControl;
	
	// configurable params
	Vec4 Color;
	PrimitiveType Type;
    double m_brushWidth;
    double m_eraseWidth;
	bool m_pressureWidth;
	bool m_pressureOpacity;
	bool m_fill;
    bool m_erase;

    drwFrameChangeMode m_frameChangeMode;
    bool m_persistenceEnabled;
    int m_persistence;
    int m_interactionStartFrame;

	double m_minWidth;
	double m_maxWidth;
	
    typedef std::map< int, int > CurrentNodesCont;  // ( frame, nodeId )
	CurrentNodesCont CurrentNodes;

    // Cursor for this tool
    Circle * m_cursor;
};

#endif
