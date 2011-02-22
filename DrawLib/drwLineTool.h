#ifndef __drwLineTool_h_
#define __drwLineTool_h_

#include "drwWidgetObserver.h"
#include <SVL.h>
#include "macros.h"
#include <map>

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
	
	virtual void MousePressEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void TabletEvent( drwDrawingWidget * w, QTabletEvent * e );
	virtual void EnterEvent( drwDrawingWidget * w, QEvent * e );
	virtual void LeaveEvent( drwDrawingWidget * w, QEvent * e );
	
	virtual void ExecuteCommand( drwCommand::s_ptr command );
	virtual void SetCurrentFrame( int frame );
	virtual void Reset();

	// Get/Set brush properties - will all generate a line tool attribute command
	bool GetPressureWidth() { return m_pressureWidth; }
	void SetPressureWidth( bool w );
	bool GetPressureOpacity() { return m_pressureOpacity; }
	void SetPressureOpacity( bool o );
	bool GetFill() { return m_fill; }
	void SetFill( bool f );
	GetMacro( Color, Vec4 );
	void SetColor( Vec4 & c );
	int GetPersistence() { return m_persistence; }
	void SetPersistence( int p );

signals:

	void ParametersChangedSignal();
	
protected:
	
	void ExecuteLineToolParamCommand( drwCommand::s_ptr command );
	void ExecuteMouseCommand( drwCommand::s_ptr command );
	void ParametersChanged();
	void CreateNewNodes();
	Node * CreateNewNode();
	
	void SetCursorPosition( drwCommand::s_ptr command );
	bool m_cursorShouldAppear;
	
	void BrushWidthStart( int x, int y );
	void BrushWidthEnd( int x, int y );
	void BrushWidthMove( int x, int y );
	
	double LastXWorld;
	double LastYWorld;
	double LastPressure;
	int m_lastXWin;
	int m_lastYWin;
	
	bool IsDrawing;
	
	// configurable params
	Vec4 Color;
	PrimitiveType Type;
	double m_baseWidth;
	bool m_pressureWidth;
	bool m_pressureOpacity;
	bool m_fill;
	int m_persistence;

	double m_minWidth;
	double m_maxWidth;
	bool m_brushScaling;
	
	typedef std::map< int, Node* > CurrentNodesCont;
	CurrentNodesCont CurrentNodes;
	
	drwEditionState * m_editionState;
};

#endif
