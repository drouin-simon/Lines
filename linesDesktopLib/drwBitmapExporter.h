#ifndef __drwBitmapExporter_h_
#define __drwBitmapExporter_h_

#include "../GraphicsEngine/GraphicsEngine.h"

#include <QThread>
#include <QSize>

class Scene;
class drwGLRenderer;
class QGLFramebufferObject;

class drwBitmapExporter : public QThread
{

	Q_OBJECT
	
public:
	
	drwBitmapExporter();
	~drwBitmapExporter();
	void SetScene( Scene * scene ) { m_scene = scene; }
	void SetFilename( QString filename ) { m_filename = filename; }
	void SetSize( const QSize & size );
    void SetExportAlpha( bool e ) { m_exportAlpha = e; }
	
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
    bool m_exportAlpha;

private:

	IGraphicsEngine* m_engine;
};

#endif
