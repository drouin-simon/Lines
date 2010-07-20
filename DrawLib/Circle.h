#ifndef __Circle_h_
#define __Circle_h_

#include "macros.h"
#include "Primitive.h"
#include "PolygonData.h"

class Circle : public Primitive
{

public:

    Circle();
    ~Circle();

    void SetCenter( Vec2 center );
    void SetCenter( double x, double y );
    SetMacro( Radius, double );
    SetMacro( Angle1, double );
    SetMacro( Angle2, double );
    SetMacro( NumberOfSubdiv, int );
    SetMacro( IntermediatePointOnly, bool);
	
	// TODO
	virtual void ExportToSWF( drwSWFWriter & writer ) {}

protected:

	virtual void InternDraw( const drwDrawingContext & context );

	void GeneratePolygonData();
	void GenerateContour();
    void GeneratePoints( drwVec2Array & points );

    Vec2   Center;
    double Radius;
    double Angle1;
    double Angle2;
    int    NumberOfSubdiv;
    bool   IntermediatePointOnly;

	// What gets drawn
	PolygonData m_poly;

};


#endif
