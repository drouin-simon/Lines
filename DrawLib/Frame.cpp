#include "Frame.h"

Frame::Frame()
{
}

Frame::~Frame()
{
}

void Frame::Clear()
{
	Nodes.clear();
}

void Frame::Draw( const drwDrawingContext & context )
{
	NodeCont::iterator it = Nodes.begin();
	while( it != Nodes.end() )
	{
		(*it)->Draw( context );
		++it;
	}
}

void Frame::AddNode( Node * node )
{
	Node::s_ptr n( node );
	Nodes.push_back( n );
}

Node * Frame::GetNodeById( GLuint Id )
{
	for( unsigned i = 0; i < Nodes.size(); ++i )
	{
		if( Nodes[i]->GetId() == Id )
			return Nodes[i].get();
	}
	return 0;
}

void Frame::ExportToSWF( drwSWFWriter & writer )
{
	for( unsigned i = 0; i < Nodes.size(); ++i )
	{
		Nodes[i]->ExportToSWF( writer );
	}
}
