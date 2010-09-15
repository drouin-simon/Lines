#include "drwBitmapExporter.h"
#include "drwDrawingWidget.h"
#include "drwDrawingContext.h"
#include <QGLFramebufferObject>

drwBitmapExporter::drwBitmapExporter()
: m_scene(0)
, m_glWidget(0)
{
}

drwBitmapExporter::~drwBitmapExporter()
{
}

void drwBitmapExporter::SetSize( const QSize & size ) 
{ 
	m_size = size; 
}

//void drwBitmapExporter::run()
//{
	//if (!QGLFormat::hasOpenGL() || !QGLPixelBuffer::hasOpenGLPbuffers())
	//{
	//	QMessageBox::information(0, "OpenGL pbuffers 2", "This system does not support OpenGL/pbuffers.");
	//	return -1;
	//}
	
//	Export();
//}

bool drwBitmapExporter::Export()
{
	if( !m_glWidget || !m_scene )
		return false;
	   
	m_glWidget->makeCurrent();
	
	QGLFramebufferObject fb( m_size );
	fb.bind();
	
	// Setup basic rendering stuff. Need more generic.
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D( 0.0, 1920.0, 0.0, 1080.0 );
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glViewport( 0, 0, m_size.width(), m_size.height() );
	
	drwDrawingContext context(m_glWidget);
	m_scene->DrawFrame( 0, context );
	
	fb.release();
	
	QImage im = fb.toImage();
	im.save( m_filename );
	
	m_glWidget->doneCurrent();
	
	/*for( int i = 0; i < 20; ++i )
	{
		emit WritingFrame( i );
		msleep( 500 );
	}*/
}
