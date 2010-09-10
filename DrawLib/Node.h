#ifndef __Node_h_
#define __Node_h_

#include <SVL.h>
#include <SVLgl.h>
#include "macros.h"
#include "SharedPtrMacro.h"

class Primitive;
class drwSWFWriter;
class drwDrawingContext;

class Node
{
	
public:
	
	SharedPtrMacro(Node);
	
	Node();
	~Node();
	
	void Draw( const drwDrawingContext & context );
	
	SetMacro( Position, Vec2 );
	GetMacro( Position, Vec2 );
	
	void SetPrimitive( Primitive * prim ) { ThePrimitive = prim; }
	Primitive * GetPrimitive() { return ThePrimitive; }
	
	GetMacro( Id, GLuint );
	
	SetMacro( IsHidden, bool );
	
protected:
	
	bool IsHidden;
	Vec2 Position;
	Primitive * ThePrimitive;

	GLuint Id;
	
	// Next id to be attributed
	static GLuint NextId;
	
};


#endif
