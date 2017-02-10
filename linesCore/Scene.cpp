#include "Scene.h"
#include "Node.h"
#include <QImage>
#include "drwGLRenderer.h"

Scene::Scene( QObject * parent )
: QObject( parent )
{
}

Scene::~Scene()
{
    // Delete all frames
    for( unsigned i = 0; i < m_frames.size(); ++i )
    {
        delete m_frames[i];
    }	
}

void Scene::Clear()
{
    m_framesLock.lockForRead();
	for( unsigned i = 0; i < m_frames.size(); ++i )
        m_frames[i]->Clear();
    m_framesLock.unlock();
    m_renderer->NeedRedraw();
}

void Scene::DrawFrame( int frame, drwDrawingContext & context )
{
    m_framesLock.lockForRead();
	if( frame < (int)m_frames.size() && frame >= 0 )
        m_frames[ frame ]->Draw( context );
    m_framesLock.unlock();
}

int Scene::AddNodeToFrame( Node * node, int frame )
{
    m_framesLock.lockForRead();
    Q_ASSERT( frame >= 0 && frame < (int)m_frames.size() );
    int nodeIndex = m_frames[ frame ]->AddNode( node );
    m_framesLock.unlock();

    return nodeIndex;
}

Node * Scene::GetNodeById( int frame, GLuint Id )
{
    m_framesLock.lockForRead();
    Node * ret = m_frames[ frame ]->GetNodeById( Id );
    m_framesLock.unlock();

	return ret;
}

Node * Scene::LockNode( int frameIndex, int nodeIndex )
{
    m_framesLock.lockForRead(); // when we lock a node, we don't want anybody to modify the frames array
    Q_ASSERT( frameIndex < m_frames.size() );
    return m_frames[ frameIndex ]->LockNode( nodeIndex );
}

void Scene::UnlockNode( int frameIndex, int nodeIndex )
{
    Q_ASSERT( frameIndex < m_frames.size() );
    m_frames[ frameIndex ]->UnlockNode( nodeIndex );
    m_framesLock.unlock();
}

void Scene::SetNumberOfFrames( int nbFrames )
{
    m_framesLock.lockForWrite();

    int currentSize = m_frames.size();
    if( currentSize != nbFrames )
    {
        if( currentSize > nbFrames )  // reduce size (delete excess frames)
        {
            for( unsigned i = nbFrames; i < currentSize; ++i )
                delete m_frames[i];
        }
        m_frames.resize( nbFrames );
        if( currentSize < nbFrames )  // increase size (allocate more)
        {
            for( unsigned i = currentSize; i < nbFrames; ++i )
                m_frames[i] = new Frame;
        }
    }

    m_framesLock.unlock();

    // Tell interface number of frames have changed
	emit NumberOfFramesChanged( nbFrames );

    // Tell db and network peers
    drwCommand::s_ptr command( new drwSceneParamsCommand( nbFrames ) );
    emit CommandExecuted( command );
}

void Scene::InsertFrame( int beforeThisFrame )
{
    m_framesLock.lockForWrite();

    Q_ASSERT( beforeThisFrame >= 0 && beforeThisFrame <= (int)m_frames.size() );

    FrameCont::iterator it = m_frames.begin();
    if( beforeThisFrame == (int)m_frames.size() )
        it = m_frames.end();
    else
        it += beforeThisFrame;
    Frame * newFrame = new Frame;
    m_frames.insert( it, newFrame );

    m_framesLock.unlock();

    emit NumberOfFramesChanged( m_frames.size() );
}

void Scene::MarkModified()
{
    m_renderer->NeedRedraw();
}


