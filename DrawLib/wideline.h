#ifndef __WideLine_h_
#define __WideLine_h_


#include "drwDataArray.h"
#include "LinePrimitive.h"

class drwGlslShader;
class drwDrawingContext;

class WideLine : public LinePrimitive
{

public:

    WideLine( double width );
    ~WideLine();
    
    virtual void StartPoint( double x, double y, double pressure );
    virtual void EndPoint( double x, double y, double pressure );
    virtual void AddPoint( double x, double y, double pressure );
	
	static void Init();

protected:
	
	virtual void InternDraw( const drwDrawingContext & context );

    // Complete description of the polygons
    drwVec2Array m_vertices;
	drwVec3Array m_texCoord;
    drwIndexArray m_indices;

    // Line width
    double      m_width;

    // cache previous point computation during editing
    Vec2       m_prevPoint;
	double	   m_prevPressure;
	
	static drwGlslShader * m_shader;
};


#endif
