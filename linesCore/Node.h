#ifndef __Node_h_
#define __Node_h_

#include "../GraphicsEngine/include/GraphicsEngineManager.h"
#include "macros.h"
#include "SharedPtrMacro.h"

class Primitive;
class drwSWFWriter;
class drwDrawingContext;

class Node
{
private:
	IGraphicsEngine* m_engine;
	
public:
	
	SharedPtrMacro(Node);
	
	Node();
	~Node();
	
    void Draw( drwDrawingContext & context );
	
	SetMacro( Position, Vec2 );
	GetMacro( Position, Vec2 );
	
	void SetPrimitive( Primitive * prim ) { ThePrimitive = prim; }
	Primitive * GetPrimitive() { return ThePrimitive; }
	
	GetMacro( Id, unsigned int );
	
	SetMacro( IsHidden, bool );
	
protected:
	
	bool IsHidden;
	Vec2 Position;
	Primitive * ThePrimitive;

	unsigned int Id;
	
	// Next id to be attributed
	static unsigned int NextId;
	
};


#endif
