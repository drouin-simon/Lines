#ifndef __WideLine_h_
#define __WideLine_h_


#include "drwDataArray.h"
#include "LinePrimitive.h"

class drwGlslShader;
class drwDrawingContext;

class WideLine : public LinePrimitive
{

public:

    WideLine();
    ~WideLine();
    
    virtual void StartPoint( double x, double y, double pressure );
    virtual void EndPoint( double x, double y, double pressure );
    virtual void AddPoint( double x, double y, double pressure );
	
	// TODO
	virtual void ExportToSWF( drwSWFWriter & writer ) {}
	
	static void Init();

protected:
	
	virtual void InternDraw( const drwDrawingContext & context );

    //void AddSegment( const Vec2 & left, const Vec2 & right, const Vec2 & newPoint );
    //void AddJoint( const Vec2 & left, const Vec2 & right, const Vec2 & newPoint );

    // Complete description of the polygons
    drwVec2Array m_vertices;
	drwVec3Array m_texCoord;
    drwIndexArray m_indices;

    // Line width
    double      m_width;

    // True only before calling AddPoint for the first time
    //bool       m_firstPoint;

    // cache previous point computation during editing
    Vec2       m_prevPoint;
	double	   m_prevPressure;
    //Vec2       m_prevDir;
    //Vec2       m_prevLeft;
    //Vec2       m_prevRight;
	
	static drwGlslShader * m_shader;
};


#endif
