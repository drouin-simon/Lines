#ifndef __Frame_h_
#define __Frame_h_

#include "macros.h"
#include <vector>
#include "Node.h"

class drwSWFWriter;

class Frame
{
		
public:
		
	Frame();
	~Frame();
	
	void Clear();
		
	void Draw( const drwDrawingContext & context );
	void AddNode( Node * node );
	Node * GetNodeById( unsigned Id );
	
	void ExportToSWF( drwSWFWriter & writer );
		
protected:
		
	typedef std::vector<Node::s_ptr> NodeCont;
	NodeCont Nodes;
		
};

#endif
