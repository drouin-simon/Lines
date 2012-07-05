#include "drwBitmapExporter.h"
#include "drwGLRenderer.h"
#include "Scene.h"
#include <QGLFramebufferObject>
#include <QGLPixelBuffer>
#include <QFileInfo>
#include <QDir>

drwBitmapExporter::drwBitmapExporter()
: m_scene(0)
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
    Q_ASSERT( m_scene );
	
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
    QGLPixelBuffer pixBuffer( m_size );
    pixBuffer.makeCurrent();

    drwGLRenderer ren;
    ren.SetCurrentScene( m_scene );
    ren.SetRenderSize( m_size.width(), m_size.height() );
    ren.SetClearColor( 0.0, 0.0, 0.0, 0.0 );
	
	int numberOfDigits = QString::number( m_scene->GetNumberOfFrames() ).size();
	
	for( int i = 0; i < m_scene->GetNumberOfFrames(); ++i )
	{
		emit WritingFrame( i );
		
        ren.Render( i, 0, 0 );

        glFlush();
	
        QImage im = pixBuffer.toImage();
		QString model( m_baseFilename );
		model.append( "%1." ).append( m_fileExtension );
		QString name = model.arg( i, numberOfDigits, 10, QChar('0') );
		im.save( name );
    }
	
    pixBuffer.doneCurrent();
}
