#include "Scene.h"
#include "Node.h"
#include "ImageSprite.h"
#include <QImage>

Scene::Scene( QObject * parent )
: QObject( parent )
, m_frameWidth( 1920.0 )
, m_frameHeight( 1080.0 )
{
}

Scene::~Scene()
{
    // Delete all frames
    for( unsigned i = 0; i < Frames.size(); ++i )
    {
        delete Frames[i];
    }

	// Delete all images
	for( unsigned i = 0; i < m_imageDb.size(); ++i )
	{
		delete m_imageDb[i].Sprite;
	}
	m_imageDb.clear();
}

void Scene::Clear()
{
    m_framesLock.lockForRead();
	for( unsigned i = 0; i < Frames.size(); ++i )
        Frames[i]->Clear();
    m_framesLock.unlock();
}

void Scene::DrawFrame( int frame, drwDrawingContext & context )
{
    m_framesLock.lockForRead();
	if( frame < (int)Frames.size() && frame >= 0 )
        Frames[ frame ]->Draw( context );
    m_framesLock.unlock();
}

int Scene::AddNodeToFrame( Node * node, int frame )
{
    m_framesLock.lockForRead();
    Q_ASSERT( frame >= 0 && frame < (int)Frames.size() );
    int nodeIndex = Frames[ frame ]->AddNode( node );
    m_framesLock.unlock();

    return nodeIndex;
}

Node * Scene::GetNodeById( int frame, GLuint Id )
{
    m_framesLock.lockForRead();
    Node * ret = Frames[ frame ]->GetNodeById( Id );
    m_framesLock.unlock();

	return ret;
}

Node * Scene::LockNode( int frameIndex, int nodeIndex )
{
    m_framesLock.lockForRead(); // when we lock a node, we don't want anybody to modify the frames array
    Q_ASSERT( frameIndex < Frames.size() );
    return Frames[ frameIndex ]->LockNode( nodeIndex );
}

void Scene::UnlockNode( int frameIndex, int nodeIndex )
{
    Q_ASSERT( frameIndex < Frames.size() );
    Frames[ frameIndex ]->UnlockNode( nodeIndex );
    m_framesLock.unlock();
}

void Scene::SetNumberOfFrames( int nbFrames )
{
    m_framesLock.lockForWrite();

    int currentSize = Frames.size();
    if( currentSize != nbFrames )
    {
        if( currentSize > nbFrames )  // reduce size (delete excess frames)
        {
            for( unsigned i = nbFrames; i < currentSize; ++i )
                delete Frames[i];
        }
        Frames.resize( nbFrames );
        if( currentSize < nbFrames )  // increase size (allocate more)
        {
            for( unsigned i = currentSize; i < nbFrames; ++i )
                Frames[i] = new Frame;
        }
    }

    m_framesLock.unlock();

	emit NumberOfFramesChanged( nbFrames );
}

void Scene::InsertFrame( int beforeThisFrame )
{
    m_framesLock.lockForWrite();

    Q_ASSERT( beforeThisFrame >= 0 && beforeThisFrame <= (int)Frames.size() );

    FrameCont::iterator it = Frames.begin();
    if( beforeThisFrame == (int)Frames.size() )
        it = Frames.end();
    else
        it += beforeThisFrame;
    Frame * newFrame = new Frame;
    Frames.insert( it, newFrame );

    m_framesLock.unlock();

    emit NumberOfFramesChanged( Frames.size() );
}

void Scene::MarkModified()
{
	emit Modified();
}

void Scene::MarkModified( int frame, Box2d & modifiedArea )
{
    emit Modified( frame, modifiedArea );
}

ImageSprite * Scene::GetImageSprite( const char * filename )
{
	// See if a sprite with same filename already exists
	for( unsigned i = 0; i < m_imageDb.size(); ++i )
	{
		if( m_imageDb[i].ImageFileName == filename )
		{
			return m_imageDb[i].Sprite;
		}
	}
	
	// It doesn't, try to load
	QImage im;
	if ( !im.load( filename ) )
		return 0;

	// Create the sprite
	ImageInfo info;
	info.ImageFileName = filename;
	info.Sprite = new ImageSprite;
	info.Sprite->SetImage( im );
	m_imageDb.push_back( info );
	
	return info.Sprite;
}

