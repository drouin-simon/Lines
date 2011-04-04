#include "ImageFlippingTool.h"
#include <limits>
#include "drwDrawingWidget.h"
#include "ImageSprite.h"
#include "Scene.h"
#include "Circle.h"
#include "Node.h"
#include <QMouseEvent>
#include <QDropEvent>
#include <QUrl>

const unsigned ImageFlippingTool::InvalidPointIndex = unsigned(-1);
const Vec4 ImageFlippingTool::PointColor( 1.0, 1.0, 1.0, 1.0 );
const Vec4 ImageFlippingTool::SelectedPointColor( 1.0, 0.0, 0.0, 1.0 );

ImageFlippingTool::ImageFlippingTool( Scene * scene, QObject * parent ) 
: drwWidgetObserver( scene, parent )
, MovingOffset(0.0,0.0)
{
	PointGeometry = new Circle;
	PointGeometry->SetCenter( Vec2(0.0,0.0) );
	PointGeometry->SetRadius(10);
	
	PrevClosestIndex = InvalidPointIndex;
	SelectedPointIndex = InvalidPointIndex;
	IsMoving = false;
	PointMoving = true;
}

ImageFlippingTool::~ImageFlippingTool()
{
	delete PointGeometry;
	for( unsigned i = 0; i < Nodes.size(); ++i )
	{
		delete Nodes[i].PointNode;
		delete Nodes[i].SpriteNode;
	}
}

void ImageFlippingTool::MousePressEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	// Deselect previously selected point
	if( SelectedPointIndex != InvalidPointIndex )
		Nodes[ SelectedPointIndex ].PointNode->GetPrimitive()->SetColor( PointColor );
	
	// Get new selection 
	Node * hit = w->Pick( e->x(), e->y() );
	double xWorld = 0.0;
	double yWorld = 0.0;
	w->WindowToWorld( e->x(), e->y(), xWorld, yWorld );
	if( hit != 0 )
	{
		for( unsigned i = 0; i < Nodes.size(); ++i )
		{
			if( Nodes[i].PointNode == hit )
			{
				SelectedPointIndex = i;
				PointMoving = true;
				IsMoving = true;
				Vec2 curPos = Nodes[i].PointNode->GetPosition();
				MovingOffset = Vec2( xWorld - curPos[0], yWorld - curPos[1] );
				break;
			}
			else if( Nodes[i].SpriteNode == hit )
			{
				SelectedPointIndex = i;
				PointMoving = false;
				IsMoving = true;
				Vec2 curPos = Nodes[i].SpriteNode->GetPosition();
				MovingOffset = Vec2( xWorld - curPos[0], yWorld - curPos[1] );
				break;
			}
		}
		// We have a newly selected point - show the corresponding image and highlight the point
		if( SelectedPointIndex != InvalidPointIndex )
		{
			Nodes[ SelectedPointIndex ].PointNode->GetPrimitive()->SetColor( SelectedPointColor );
			DisplayIndex( SelectedPointIndex );
		}
		// No selected point - display nearest neighbor
		else
		{
			SelectedPointIndex = InvalidPointIndex;
			DisplayClosest( Vec2( xWorld, yWorld ) );
		}
	}
	else
	{
		SelectedPointIndex = InvalidPointIndex;
		DisplayClosest( Vec2( xWorld, yWorld ) );
	}
}

void ImageFlippingTool::MouseReleaseEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	if( IsMoving )
		IsMoving = false;
}

void ImageFlippingTool::MouseMoveEvent( drwDrawingWidget * w, QMouseEvent * e )
{
	double xWorld = 0.0;
	double yWorld = 0.0;
	w->WindowToWorld( e->x(), e->y(), xWorld, yWorld );
	if( SelectedPointIndex != InvalidPointIndex )
	{
		if( IsMoving )
		{
			Vec2 newPos = Vec2( xWorld, yWorld ) - MovingOffset;
			if( PointMoving )
				Nodes[ SelectedPointIndex ].PointNode->SetPosition( newPos );
			else
				Nodes[ SelectedPointIndex ].SpriteNode->SetPosition( newPos );
		}
	}
	else
	{
		// Find the closest sprite and set it as the default sprite in the scene if different than the current one.
		unsigned closestIndex = ComputeNearestNeighbor( Vec2( xWorld, yWorld ) );

		if( closestIndex != InvalidPointIndex && closestIndex != PrevClosestIndex )
		{
			if( PrevClosestIndex != InvalidPointIndex )
				Nodes[ PrevClosestIndex ].SpriteNode->SetIsHidden( true );
			Nodes[ closestIndex ].SpriteNode->SetIsHidden( false );
			PrevClosestIndex = closestIndex;
		}
	}
}

void ImageFlippingTool::DisplayIndex( unsigned index )
{
	for( unsigned i = 0; i < Nodes.size(); ++i )
	{
		bool hidden = i == index ? false : true;
		Nodes[i].SpriteNode->SetIsHidden( hidden );
	}
}

void ImageFlippingTool::DisplayClosest( Vec2 pos )
{
	unsigned index = ComputeNearestNeighbor(pos);
	DisplayIndex( index );
}

unsigned ImageFlippingTool::ComputeNearestNeighbor( Vec2 pos )
{
	double shortestDist = std::numeric_limits<double>::infinity();
	unsigned closestIndex = InvalidPointIndex;
	for( unsigned i = 0; i < Nodes.size(); ++i )
	{
		Vec2 nodePos = Nodes[i].PointNode->GetPosition();
		double diffX = pos[0] - nodePos[0];
		double diffY = pos[1] - nodePos[1];
		double sqDist = diffX * diffX + diffY * diffY;
		if( sqDist < shortestDist )
		{
			shortestDist = sqDist;
			closestIndex = i;
		}
	}
	return closestIndex;
}

void ImageFlippingTool::DropEvent( drwDrawingWidget * w, QDropEvent * e )
{
	double xWorld = 0.0;
	double yWorld = 0.0;
	w->WindowToWorld( e->pos().x(), e->pos().y(), xWorld, yWorld );
	
	const QMimeData * mimeData = e->mimeData();
		
	// Only accept a filename
	if (mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();
		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			QString url = urlList.at(i).toLocalFile();
			ImageSprite * sprite = CurrentScene->GetImageSprite( url.toAscii() );
			
			if( sprite )
			{
				// Create a node for the droped point and image
				NodeInfo newNode;
				newNode.PointNode = new Node;
				newNode.PointNode->SetPrimitive( PointGeometry );
				newNode.PointNode->SetPosition( Vec2( xWorld, yWorld ) );
                CurrentScene->AddNodeToFrame(newNode.PointNode, 0 );
				newNode.SpriteNode = new Node;
				newNode.SpriteNode->SetPrimitive( sprite );
                CurrentScene->AddNodeToFrame(newNode.SpriteNode, 0 );
				Nodes.push_back( newNode );
			}
		}
	}
}

void ImageFlippingTool::Reset()
{
	MovingOffset = Vec2(0.0,0.0);
	PrevClosestIndex = InvalidPointIndex;
	SelectedPointIndex = InvalidPointIndex;
	IsMoving = false;
	PointMoving = true;
}
