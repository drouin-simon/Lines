#include "drwBitmapExporter.h"
#include "drwDrawingWidget.h"
#include "drwDrawingContext.h"
#include <QGLFramebufferObject>
#include <QFileInfo>
#include <QDir>

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

bool drwBitmapExporter::StartWriting()
{
	// make sure we have a scene and widget
	if( !m_glWidget || !m_scene )
		return false;
	
	// Check validity of the filename
	QFileInfo fileInfo( m_filename );
	QString dir = fileInfo.dir().path();
	m_baseFilename = dir;
	m_baseFilename += "/";
	m_baseFilename += fileInfo.completeBaseName();
	m_fileExtension = fileInfo.suffix();
	
	// start the thread
	start();
	
	return true;
}

void drwBitmapExporter::run()
{
	m_glWidget->makeCurrent();
	
	m_fb = new QGLFramebufferObject( m_size );
	m_fb->bind();
	
	// Save drawing window's viewport
	glPushAttrib( GL_VIEWPORT_BIT );
	
	int numberOfDigits = QString::number( m_scene->GetNumberOfFrames() ).size();
	
	for( int i = 0; i < m_scene->GetNumberOfFrames(); ++i )
	{
		emit WritingFrame( i );
		
		// Setup basic rendering stuff. Need more generic.
		glClearColor( 0.0, 0.0, 0.0, 1.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
        gluOrtho2D( 0.0, m_scene->GetFrameWidth(), 0.0, m_scene->GetFrameHeight() );
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glViewport( 0, 0, m_size.width(), m_size.height() );
		
		drwDrawingContext context(m_glWidget);
		m_scene->DrawFrame( i, context );
	
		QImage im = m_fb->toImage();
		QString model( m_baseFilename );
		model.append( "%1." ).append( m_fileExtension );
		QString name = model.arg( i, numberOfDigits, 10, QChar('0') );
		im.save( name );
	}
	
	// retrieve drawing window's viewport
	glPopAttrib();
	
	m_fb->release();
	delete m_fb;
	m_fb = 0;
	
	m_glWidget->doneCurrent();

}
