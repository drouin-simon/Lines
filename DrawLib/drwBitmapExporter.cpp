#include "drwBitmapExporter.h"

drwBitmapExporter::drwBitmapExporter()
{
	m_scene = 0;
}

drwBitmapExporter::~drwBitmapExporter()
{
}

void drwBitmapExporter::run()
{
	for( int i = 0; i < 20; ++i )
	{
		emit WritingFrame( i );
		msleep( 500 );
	}
}