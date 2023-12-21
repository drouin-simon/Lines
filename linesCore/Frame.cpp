#include "Frame.h"

Frame::Frame()
{
}

Frame::~Frame()
{
    Clear();
}

void Frame::Clear()
{
    m_nodesLock.lockForWrite();
    NodeCont::iterator it = Nodes.begin();
    while( it != Nodes.end() )
    {
        delete (*it);
        ++it;
    }
	Nodes.clear();
    m_nodesLock.unlock();
}

void Frame::Draw( drwDrawingContext & context )
{
    m_nodesLock.lockForRead();
	NodeCont::iterator it = Nodes.begin();
	while( it != Nodes.end() )
	{
        (*it)->Draw( context );
		++it;
	}
    m_nodesLock.unlock();
}

int Frame::AddNode( Node * node )
{
    m_nodesLock.lockForWrite();
    int nodeId = Nodes.size();
    Nodes.push_back( node );
    m_nodesLock.unlock();

    return nodeId;
}

Node * Frame::GetNodeById(unsigned int Id )
{
	for( unsigned i = 0; i < Nodes.size(); ++i )
	{
		if( Nodes[i]->GetId() == Id )
            return Nodes[i];
	}
	return 0;
}

Node * Frame::LockNode( int nodeIndex )
{
    m_nodesLock.lockForWrite();
    Q_ASSERT( nodeIndex < Nodes.size() );
    return Nodes[ nodeIndex ];
}

void Frame::UnlockNode( int /*nodeIndex*/ )
{
    m_nodesLock.unlock();
}
