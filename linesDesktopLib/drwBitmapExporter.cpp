#include "drwBitmapExporter.h"
#include "drwGLRenderer.h"
#include "Scene.h"
#include <QFileInfo>
#include <QDir>
#include <QSurfaceFormat>
#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLFramebufferObject>

drwBitmapExporter::drwBitmapExporter()
: m_scene(0)
{
    m_exportAlpha = false;
    m_engine = new GraphicsEngine();
}

drwBitmapExporter::~drwBitmapExporter()
{
    delete m_engine;
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
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(2);

    QWindow window;
    window.setSurfaceType(QWindow::OpenGLSurface);
    window.setFormat(format);
    window.create();

    QOpenGLContext context;
    context.setFormat(format);
    if (!context.create())
        qFatal("Cannot create the requested OpenGL context!");
    context.makeCurrent(&window);

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setSamples(16);
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLFramebufferObject fbo( m_size, fboFormat);

    drwGLRenderer ren;
    ren.SetCurrentScene( m_scene );
    ren.SetRenderSize( m_size.width(), m_size.height() );
    double clearAlpha = m_exportAlpha ? 0.0 : 1.0;
    ren.SetClearColor( 0.0, 0.0, 0.0, clearAlpha );
	
	int numberOfDigits = QString::number( m_scene->GetNumberOfFrames() ).size();
	
	for( int i = 0; i < m_scene->GetNumberOfFrames(); ++i )
	{
        fbo.bind();

		emit WritingFrame( i );
		
        ren.RenderToTexture( i );
        ren.RenderTextureToScreen();

        m_engine->Flush();

        fbo.release();
        QImage im = fbo.toImage();
		QString model( m_baseFilename );
		model.append( "%1." ).append( m_fileExtension );
		QString name = model.arg( i, numberOfDigits, 10, QChar('0') );
		im.save( name );
    }
}
