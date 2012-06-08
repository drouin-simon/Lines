#ifndef __drwLineTool_h_
#define __drwLineTool_h_

#include "drwWidgetObserver.h"
#include <SVL.h>
#include "macros.h"
#include <map>
#include "Box2d.h"

class Node;
class LinePrimitive;
class drwEditionState;

enum PrimitiveType { TypeLine = 0, TypeWideLine, EndType };

class drwLineTool : public drwWidgetObserver
{
	
	Q_OBJECT

public:
	
	drwLineTool( Scene * scene, drwEditionState * editionState, QObject * parent = 0 );
	virtual ~drwLineTool() {};

    // convenience functions used to automatically create lines
    void StartLine( double xWorld, double yWorld );
    void AddPoint( double xWorld, double yWorld );
    void EndLine( double xWorld, double yWorld );
	
	virtual void MousePressEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void TabletEvent( drwDrawingWidget * w, QTabletEvent * e );
	
	virtual void ExecuteCommand( drwCommand::s_ptr command );
	virtual void SetCurrentFrame( int frame );
	virtual void Reset();

	// Get/Set brush properties - will all generate a line tool attribute command
	bool GetPressureWidth() { return m_pressureWidth; }
	void SetPressureWidth( bool w );
	bool GetPressureOpacity() { return m_pressureOpacity; }
	void SetPressureOpacity( bool o );
    bool IsPerssureWidthAndOpacityEnabled();
	bool GetFill() { return m_fill; }
	void SetFill( bool f );
	GetMacro( Color, Vec4 );
	void SetColor( Vec4 & c );
	int GetPersistence() { return m_persistence; }
	void SetPersistence( int p );
    bool IsPersistenceEnabled();
    void SetPersistenceEnabled( bool on );
    void SetBaseWidth( double bw );
    double GetBaseWidth() { return m_baseWidth; }

signals:

	void ParametersChangedSignal();
	
protected:
	
	void ExecuteLineToolParamCommand( drwCommand::s_ptr command );
	void ExecuteMouseCommand( drwCommand::s_ptr command );
	void ParametersChanged();
	void CreateNewNodes();
	Node * CreateNewNode();
	
    double m_lastXWorld;
    double m_lastYWorld;
    double m_lastPressure;
    int m_lastXPix;
    int m_lastYPix;
	
	bool IsDrawing;
    Box2d m_lastModifBox;
	
	// configurable params
	Vec4 Color;
	PrimitiveType Type;
	double m_baseWidth;
	bool m_pressureWidth;
	bool m_pressureOpacity;
	bool m_fill;
	int m_persistence;

    double m_minDistanceBetweenPoints;
	double m_minWidth;
	double m_maxWidth;
	
    typedef std::map< int, int > CurrentNodesCont;  // ( frame, nodeId )
	CurrentNodesCont CurrentNodes;
	
	drwEditionState * m_editionState;
};

#endif
