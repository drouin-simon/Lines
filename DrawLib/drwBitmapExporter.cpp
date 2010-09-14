#include "drwBitmapExporter.h"
#include <QGLFormat>
#include <QGLPixelBuffer>

drwBitmapExporter::drwBitmapExporter()
{
	m_scene = 0;
}

drwBitmapExporter::~drwBitmapExporter()
{
}

void drwBitmapExporter::run()
{
	//if (!QGLFormat::hasOpenGL() || !QGLPixelBuffer::hasOpenGLPbuffers())
	//{
	//	QMessageBox::information(0, "OpenGL pbuffers 2", "This system does not support OpenGL/pbuffers.");
	//	return -1;
	//}

	/*QGLPixelBuffer * pbuffer = new QGLPixelBuffer(1024, 1024);
	pbuffer->makeCurrent();

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, 1024, 0, 1024 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glClearColor( 0.0, 0.0, 1.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT );

	glColor4d( 1.0, 0.0, 0.0, 1.0 );
	glBegin(GL_QUADS);
	{
		glVertex2d( 100, 100 );
		glVertex2d( 600, 100 );
		glVertex2d( 600, 600 );
		glVertex2d( 100, 600 );
	}

	glFlush();

	pbuffer->doneCurrent();

	QImage im = pbuffer->toImage();
	im.save( "/home/simon/test.png" );

	delete pbuffer;*/

	for( int i = 0; i < 20; ++i )
	{
		emit WritingFrame( i );
		msleep( 500 );
	}
}
