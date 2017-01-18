#ifndef POLYGONDATA_H
#define POLYGONDATA_H

#include "drwDataArray.h"

class PolygonData
{

public:

    PolygonData(){}
    ~PolygonData(){}
	
	bool IsEmpty() { return m_points.size() == 0; }
	void Clear() { m_points.clear(); m_triangles.clear(); m_lines.clear(); }

    drwVec2Array  & GetPoints()    { return m_points; }
    drwIndexArray & GetTriangles() { return m_triangles; }
    drwIndexArray & GetLines()     { return m_lines; }

private:

    drwVec2Array m_points;
    drwIndexArray m_triangles;
    drwIndexArray m_lines;
};


#endif