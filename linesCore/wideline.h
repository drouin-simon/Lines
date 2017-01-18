#ifndef __WideLine_h_
#define __WideLine_h_

#include "drwDataArray.h"
#include "LinePrimitive.h"
#include "Box2d.h"

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

	void SetPressureWidth( bool w ) { m_pressureWidth = w; }
	void SetPressureOpacity( bool o ) { m_pressureOpacity = o; }
	void SetFill( bool f ) { m_fill = f; }
    void SetErase( bool erase ) { m_erase = erase; }
	
    void InitShader( drwDrawingContext & context );
    
    static void SetPixMargin( float pixMargin ) { m_pix_margin = pixMargin; }
    static float GetPixMargin() { return m_pix_margin; }
    static void SetPixDampWidth( float pixDampWidth ) { m_pix_damp_width = pixDampWidth; }
    static float GetPixDampWidth() { return m_pix_damp_width; }
    static void SetSigmaLarge( float sigmaLarge ) { m_sigma_large = sigmaLarge; if( sigmaLarge < m_sigma_small ) m_sigma_small = m_sigma_large; }
    static float GetSigmaLarge() { return m_sigma_large; }
    static void SetSigmaSmall( float ss ) { m_sigma_small = ss; if( m_sigma_large < m_sigma_small ) m_sigma_large = m_sigma_small; }
    static float GetSigmaSmall() { return m_sigma_small; }

protected:
	
    virtual void InternDraw( drwDrawingContext & context );
    void AddLinePoint( double x, double y, double pressure );
	void AddFillPoint( double x, double y );

	// WideLine
    drwVec2Array m_vertices;
	drwVec3Array m_texCoord;
    drwVec3Array m_normals;
    drwIndexArray m_indices;

	// Fill
	drwVec2Array m_fillVertices;
	drwIndexArray m_fillIndices;

    // Line attributes
    double m_width;
	bool m_pressureWidth;
	bool m_pressureOpacity;
	bool m_fill;
    bool m_erase;
    Box2d m_boundingBox;
    
    // Global attributes for all lines
    static float m_pix_margin;
    static float m_pix_damp_width;
    static float m_sigma_large;
    static float m_sigma_small;

    // cache previous point computation during editing
    Vec2       m_prevPoint;
	double	   m_prevPressure;
	bool m_doneAddingPoints;
};


#endif
