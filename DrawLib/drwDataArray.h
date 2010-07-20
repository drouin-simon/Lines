#ifndef __drwDataArray_h_
#define __drwDataArray_h_

#include <vector>
#include <SVL.h>
#include <SVLgl.h>

template< class T >
class drwDataArray : public std::vector< T >
{

public:

    T * GetBuffer()			{ return (T*)&(this->front()); }
	T * GetLastN( int n )   { return ((T*)(&(this->back()) - n + 1 )); }
};

template< class T >
class drwVecArray : public std::vector< T >
{
	
public:
	
    // Get pointers to double arrays for use in opengl
	double * GetBuffer()            { return this->front().Ref(); }
    double * GetVector( int index ) { return (*this)[index].Ref(); }
    double * GetLastN( int n )      { return ((double*)(&(this->back()) - n + 1 )); }
	
};


typedef drwVecArray< Vec2 > drwVec2Array;
typedef drwVecArray< Vec3 > drwVec3Array;
typedef drwDataArray< unsigned > drwIndexArray;
typedef drwDataArray< double > drwDoubleArray;

#endif