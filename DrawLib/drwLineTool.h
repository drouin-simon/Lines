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
	virtual void ToggleBrushEraser();
	
	SetMacro( Color, Vec3 );
	
protected:
	
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
	
	bool IsDrawing;
	PrimitiveType Type;
	bool m_isEraser;
	
	Vec3 Color;
	double m_baseWidth;
	double m_minWidth;
	double m_maxWidth;
	int m_lastXWin;
	int m_lastYWin;
	bool m_brushScaling;
	
	typedef std::map< int, Node* > CurrentNodesCont;
	CurrentNodesCont CurrentNodes;
	
	drwEditionState * m_editionState;
};

#endif
