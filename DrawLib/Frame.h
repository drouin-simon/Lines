#ifndef __Frame_h_
#define __Frame_h_

//#include "macros.h"
#include <vector>
#include "Node.h"
#include <QReadWriteLock>

class Frame
{
		
public:
		
	Frame();
	~Frame();
	
	void Clear();
		
    void Draw( drwDrawingContext & context );
    int AddNode( Node * node );
	Node * GetNodeById( unsigned Id );

    Node * LockNode( int nodeIndex );
    void UnlockNode( int nodeIndex );
		
protected:
		
    typedef std::vector<Node*> NodeCont;
	NodeCont Nodes;
    QReadWriteLock m_nodesLock;
		
};

#endif
