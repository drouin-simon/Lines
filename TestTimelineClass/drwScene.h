#ifndef __drwScene_h_
#define __drwScene_h_

#include <vector>

class drwScene
{
	
public:
	
	class Layer
	{
	public:
		Layer( int start, int length ) : m_start(start), m_length( length ) {}
		Layer() : m_start( 0 ), m_length( 0 ) {}
		int m_start;
		int m_length;
	};
	
	drwScene();
	~drwScene();
	
	int GetSceneLength();
	int GetNbLayers();
	void GetLayerBounds( int layerIndex, int & layerStart, int & layerLength );
	void MoveLayer( int layerIndex, int offset );
	
protected:
	
	std::vector<Layer> m_layers;
	
};

#endif