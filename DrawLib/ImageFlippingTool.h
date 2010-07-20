#ifndef __ImageFlippingTool_h_
#define __ImageFlippingTool_h_

#include "drwWidgetObserver.h"
#include <vector>
#include <string>
#include <SVL.h>

class Node;
class Circle;

class ImageFlippingTool : public drwWidgetObserver
{
	
	Q_OBJECT

public:

	ImageFlippingTool( Scene * scene, QObject * parent = 0 );
	virtual ~ImageFlippingTool();

	virtual void MousePressEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e );
	virtual void DropEvent( drwDrawingWidget * w, QDropEvent * e );
	
	virtual void Reset();

protected:
	
	void DisplayIndex( unsigned index );
	void DisplayClosest( Vec2 pos );	
	unsigned ComputeNearestNeighbor( Vec2 pos );

	// One node per point. The node contains the transform of the point and of the image
	struct NodeInfo
	{
		int SpriteIndex;
		Node * PointNode;
		Node * SpriteNode;
	};
	std::vector< NodeInfo > Nodes;
	
	// The point geometry (only one, each point being drawn with this one)
	Circle * PointGeometry;
	
	// Vars for image flipping
	unsigned PrevClosestIndex;
	
	// Vars for point and image manipulation
	unsigned SelectedPointIndex;
	bool IsMoving;
	bool PointMoving;
	Vec2 MovingOffset;

	static const unsigned InvalidPointIndex;
	static const Vec3 PointColor;
	static const Vec3 SelectedPointColor;

};

#endif


