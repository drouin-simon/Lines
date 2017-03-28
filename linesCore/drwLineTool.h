#ifndef __drwLineTool_h_
#define __drwLineTool_h_

#include "drwTool.h"
#include <SVL.h>
#include "macros.h"
#include <map>

class Node;
class LinePrimitive;
class drwToolbox;

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

	// Get/Set brush properties - will all generate a line tool attribute command
	bool GetPressureWidth() { return m_pressureWidth; }
	void SetPressureWidth( bool w );
	bool GetPressureOpacity() { return m_pressureOpacity; }
	void SetPressureOpacity( bool o );
    bool IsPerssureWidthAndOpacityEnabled();
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
    double GetBaseWidth() { return m_baseWidth; }

signals:

    void CommandExecuted( drwCommand::s_ptr command );
	void ParametersChangedSignal();
	
protected:
	
	void ExecuteLineToolParamCommand( drwCommand::s_ptr command );
	void ExecuteMouseCommand( drwCommand::s_ptr command );
	void ParametersChanged();
	void CreateNewNodes();
	Node * CreateNewNode();
    void MarkSegmentModified( double x1, double y1, double x2, double y2 );
    void MarkPointModified( double x, double y );
    void MarkWholePrimitiveModified();
	
    double m_lastXWorld;
    double m_lastYWorld;
    double m_lastPressure;
    int m_lastXPix;
    int m_lastYPix;
	
    bool m_isDrawing;
    
    // prevents mouse commands to be generated while tablet is active
    // for drivers that send both mouse and tablet events to the program
    bool m_tabletHasControl;
	
	// configurable params
	Vec4 Color;
	PrimitiveType Type;
	double m_baseWidth;
	bool m_pressureWidth;
	bool m_pressureOpacity;
	bool m_fill;
    bool m_erase;

    drwFrameChangeMode m_frameChangeMode;
    drwFrameChangeMode m_backupFrameChangeMode;
    bool m_persistenceEnabled;
    int m_persistence;
    int m_interactionStartFrame;

	double m_minWidth;
	double m_maxWidth;
	
    typedef std::map< int, int > CurrentNodesCont;  // ( frame, nodeId )
	CurrentNodesCont CurrentNodes;
};

#endif
