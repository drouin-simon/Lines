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
	
	drwLineTool( int userId, Scene * scene, drwEditionState * editionState, QObject * parent = 0 );
	virtual ~drwLineTool() {};
	
	virtual void MousePressEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void TabletEvent( drwDrawingWidget * w, QTabletEvent * e );
	virtual void ExecuteCommand( drwCommand::s_ptr command );
	virtual void SetCurrentFrame( int frame );
	virtual void Reset();
	
	SetMacro( Color, Vec3 );
	
protected:
	
	void CreateNewNodes();
	Node * CreateNewNode();
	
	double LastXWorld;
	double LastYWorld;
	double LastPressure;
	
	bool IsDrawing;
	PrimitiveType Type;
	
	Vec3 Color;
	
	typedef std::map< int, Node* > CurrentNodesCont;
	CurrentNodesCont CurrentNodes;
	
	drwEditionState * m_editionState;
	
	int m_userId;
	
};

#endif
