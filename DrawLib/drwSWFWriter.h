#ifndef __drwSWFWriter_h_
#define __drwSWFWriter_h_

#include <string>
#include <list>

class SWFMovie;
class SWFMovieClip;
class SWFShape;
class SWFDisplayItem;

class drwSWFWriter
{
	
public:
	
	drwSWFWriter();
	~drwSWFWriter();
	
	void SetFrameRate( int frameRate ) { FrameRate = frameRate; }
	void SetNumberOfFrames( int nbFrames ) { NumberOfFrames = nbFrames; }
	void SetDimensions( double xAnim, double yAnim, double widthAnim, double heightAnim, int widthOutput, int heightOutput )
	{
		XAnim = xAnim;
		YAnim = yAnim;
		WidthAnim = widthAnim;
		HeightAnim = heightAnim;
		WidthOutput = widthOutput;
		HeightOutput = heightOutput;
	}
	
	// Functions above must be called before Initialize
	void Initialize();
	
	void StartLine( int width, double r, double g, double b );
	void MovePenTo( double x, double y );
	void DrawLineTo( double x, double y );
	void EndLine();
	
	void NextFrame();
	
	void SetFilename( const char * filename ) { Filename = filename; }
	void Write();
	
protected:
	
	void Clear();
	void ScreenToFile( double xScreen, double yScreen, double & xFile, double & yFile );
	
	std::string Filename;
	
	int FrameRate;
	int NumberOfFrames;
	double XAnim;
	double YAnim;
	double WidthAnim;
	double HeightAnim;
	int WidthOutput;
	int HeightOutput;
	
	SWFMovie * Movie;
	SWFMovieClip * Clip;
	SWFShape * CurrentLine;
	typedef std::list< SWFDisplayItem * > FrameLinesList;
	FrameLinesList FrameLines;
	
	static bool mingIsInit;
};

#endif