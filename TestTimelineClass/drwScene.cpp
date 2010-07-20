#include "drwScene.h"

drwScene::drwScene() 
{
	m_layers.push_back( Layer( 5, 5 ) );
	m_layers.push_back( Layer( 0, 11 ) );
}

drwScene::~drwScene() {}

int drwScene::GetSceneLength()
{
	int max = 0;
	for( unsigned i = 0; i < m_layers.size(); ++i )
	{
		int end = m_layers[i].m_start + m_layers[i].m_length;
		if( end > max )
			max = end;
	}
	return max;
}

int drwScene::GetNbLayers()
{
	return (int)m_layers.size();
}

void drwScene::GetLayerBounds( int layerIndex, int & layerStart, int & layerLength )
{
	if( layerIndex < (int)m_layers.size() )
	{
		layerStart = m_layers[layerIndex].m_start;
		layerLength = m_layers[layerIndex].m_length;
	}
	else 
	{
		layerStart = 0;
		layerLength = 0;
	}
}

void drwScene::MoveLayer( int layerIndex, int offset )
{
	if( layerIndex < (int)m_layers.size() )
	{
		m_layers[ layerIndex ].m_start += offset;
		if( m_layers[ layerIndex ].m_start < 0 )
			m_layers[ layerIndex ].m_start = 0;
	}
}
