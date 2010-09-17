#ifndef __drwBitmapExporter_h_
#define __drwBitmapExporter_h_

#include <QThread>
#include <QSize>

class Scene;
class drwDrawingWidget;
class QGLFramebufferObject;

class drwBitmapExporter : public QThread
{

	Q_OBJECT
	
public:
	
	drwBitmapExporter();
	~drwBitmapExporter();
	void SetScene( Scene * scene ) { m_scene = scene; }
	void SetFilename( QString filename ) { m_filename = filename; }
	void SetGLWidget( drwDrawingWidget * glWidget ) { m_glWidget = glWidget; }
	void SetSize( const QSize & size );
	
	bool StartWriting();
	
	virtual void run();
	
signals:
	
	void WritingFrame( int frame );
	
	
protected:
	
	Scene * m_scene;
	QString m_filename;
	QString m_baseFilename;
	QString m_fileExtension;
	QSize m_size;
	drwDrawingWidget * m_glWidget;
	QGLFramebufferObject * m_fb;
	
};
#endif