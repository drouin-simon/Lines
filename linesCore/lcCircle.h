#ifndef __Circle_h_
#define __Circle_h_

#include "../GraphicsEngine/include/GraphicsEngineManager.h"
#include "macros.h"
#include "Primitive.h"
#include "PolygonData.h"

class Circle : public Primitive
{

public:

    Circle();

    void SetCenter( Vec2 center );
    void SetCenter( double x, double y );
    void SetRadius( double r );
    SetMacro( Angle1, double );
    SetMacro( Angle2, double );
    SetMacro( NumberOfSubdiv, int );
    SetMacro( IntermediatePointOnly, bool);
	SetMacro( Contour, bool );
	SetMacro( Fill, bool );

    virtual const Box2d & BoundingBox();

protected:

    virtual void InternDraw( drwDrawingContext & context );

	void GeneratePolygonData();
	void GenerateContour();
    void GeneratePoints( drwVec2Array & points );
    void MarkModified();

    Vec2   Center;
    double Radius;
    double Angle1;
    double Angle2;
    int    NumberOfSubdiv;
    bool   IntermediatePointOnly;
	bool   Contour;
	bool   Fill;

	// What gets drawn
	PolygonData m_poly;

private:
    IGraphicsEngine* m_engine;
};


#endif
