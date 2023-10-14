#include "IGraphicsEngine.h"
#include "Node.h"
#include "Primitive.h"
#include "drwDrawingContext.h"

GLuint Node::NextId = 1;

Node::Node() : Position(0,0)
{
	Id = NextId++;
	ThePrimitive = 0;
	IsHidden = false;
}


Node::~Node()
{
}


void Node::Draw( drwDrawingContext & context )
{
	if( ThePrimitive && !IsHidden )
	{
		if( context.m_isPicking )
			glPushName(Id);

		glPushMatrix();
		
		//glTranslate( Position );

		ThePrimitive->Draw( context );
		
		glPopMatrix();
		
		if( context.m_isPicking )
			glPopName();
	}
}

