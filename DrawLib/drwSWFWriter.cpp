#include "drwSWFWriter.h"
#include "mingpp.h"

bool drwSWFWriter::mingIsInit = false;

drwSWFWriter::drwSWFWriter() 
: Movie(0)
, Clip(0)
, CurrentLine(0)
{
}

drwSWFWriter::~drwSWFWriter()
{
	Clear();
}

void drwSWFWriter::Clear()
{
	delete CurrentLine;
	delete Clip;
	delete Movie;
}

void drwSWFWriter::Initialize()
{
	Clear();
	
	// Init Ming
	if( !mingIsInit )
	{
		Ming_init();
		mingIsInit = true;
	}
	
	Movie = new SWFMovie();
	Movie->setDimension( WidthOutput, HeightOutput );
	Movie->setRate( FrameRate );
	Movie->setNumberOfFrames( NumberOfFrames );
	Movie->setBackground( 0, 0, 0 );
	
	Clip = new SWFMovieClip;
	Clip->setNumberOfFrames( NumberOfFrames );
}

void drwSWFWriter::StartLine( int width, double r, double g, double b )
{
	CurrentLine = new SWFShape();
	CurrentLine->setLine( width, (unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255) );
}

void drwSWFWriter::MovePenTo( double x, double y )
{
	if( CurrentLine )
	{
		double xOut = 0;
		double yOut = 0;
		ScreenToFile( x, y, xOut, yOut );
		CurrentLine->movePenTo( xOut, yOut );
	}
}

void drwSWFWriter::DrawLineTo( double x, double y )
{
	if( CurrentLine )
	{
		double xOut = 0;
		double yOut = 0;
		ScreenToFile( x, y, xOut, yOut );
		CurrentLine->drawLineTo( xOut, yOut );
	}
}

void drwSWFWriter::EndLine()
{
	if( CurrentLine )
	{
		CurrentLine->end();
		SWFDisplayItem * line = Clip->add( CurrentLine );
		FrameLines.push_back( line );
		CurrentLine = 0;
	}
}

void drwSWFWriter::NextFrame()
{
	Clip->nextFrame();
	FrameLinesList::iterator it = FrameLines.begin();
	while( it != FrameLines.end() )
	{
		Clip->remove( *it );
		++it;
	}
	FrameLines.clear();
}

void drwSWFWriter::Write()
{
	Movie->add( Clip );
	Movie->nextFrame();
	Movie->add( new SWFAction("gotoFrame(0);") );
	Movie->nextFrame();
	Movie->save(Filename.c_str());
}

void drwSWFWriter::ScreenToFile( double xScreen, double yScreen, double & xFile, double & yFile )
{
	xFile = ( xScreen - XAnim ) * WidthOutput / WidthAnim;
	yFile = ( HeightAnim - (yScreen - YAnim) ) * HeightOutput / HeightAnim;
}
