#ifndef __Scene_h_
#define __Scene_h_

#include "macros.h"
#include <vector>
#include <QObject>
#include <QMutex>
#include "Frame.h"

class ImageSprite;

class Scene : public QObject
{
	
Q_OBJECT
	
public:
	
	Scene( QObject * parent = 0 );
	~Scene();
	
	void Clear();
	
	void DrawPersistent( const drwDrawingContext & context );
	void DrawFrame( int frame, const drwDrawingContext & context );
	
	void AddNode( Node * node );
	void AddNodeToFrame( Node * node, int frame );
	Node * GetNodeById( int frame, unsigned Id );
	
	void SetNumberOfFrames( int nbFrames );
	int GetNumberOfFrames() { return Frames.size(); }
	
	// used by other classes to mark end of modification and notify clients they can re-render
	void MarkModified();
	
	void ExportToSWF( const char * filename, int width, int height );
	
	ImageSprite * GetImageSprite( const char * filename );
	
signals:
	
	void Modified();
	void NumberOfFramesChanged(int);
	
public slots:
	
	void InsertFrame( int beforeThisFrame );
	
protected:
	
	// Part of the scene that is displayed for any frame
	Frame  AlwaysDisplayed;
	
	// Only one at a time is displayed
	QMutex m_nodesMutex;
	std::vector<Frame> Frames;
	
	// Database of images used throughout the scene
	struct ImageInfo
	{
		ImageSprite * Sprite;
		std::string ImageFileName;
	};
	std::vector< ImageInfo > m_imageDb;

};

#endif
