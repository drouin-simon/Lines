#ifndef __Scene_h_
#define __Scene_h_

#include "macros.h"
#include <vector>
#include <QObject>
#include <QReadWriteLock>
#include "Frame.h"

class ImageSprite;
class drwCursor;

class Scene : public QObject
{
	
Q_OBJECT
	
public:
	
	Scene( QObject * parent = 0 );
	~Scene();
	
	void Clear();
	
	void DrawFrame( int frame, const drwDrawingContext & context );
	
    int AddNodeToFrame( Node * node, int frame );
	Node * GetNodeById( int frame, unsigned Id );

    // Lock a node in a frame for writing
    Node * LockNode( int frameIndex, int nodeIndex );
    void UnlockNode( int frameIndex, int nodeIndex );
	
	void SetNumberOfFrames( int nbFrames );
	int GetNumberOfFrames() { return Frames.size(); }
	
	// used by other classes to mark end of modification and notify clients they can re-render
	void MarkModified();
	
	ImageSprite * GetImageSprite( const char * filename );
	
signals:
	
	void Modified();
	void NumberOfFramesChanged(int);
	
public slots:
	
	void InsertFrame( int beforeThisFrame );
	
protected:
	
    QReadWriteLock m_framesLock;
    typedef std::vector<Frame*> FrameCont;
    FrameCont Frames;
	
	// Database of images used throughout the scene
	struct ImageInfo
	{
		ImageSprite * Sprite;
		std::string ImageFileName;
	};
	std::vector< ImageInfo > m_imageDb;

};

#endif
