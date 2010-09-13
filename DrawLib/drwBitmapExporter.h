#ifndef __drwBitmapExporter_h_
#define __drwBitmapExporter_h_

#include <QThread>

class Scene;

class drwBitmapExporter : public QThread
{

	Q_OBJECT
	
public:
	
	drwBitmapExporter();
	~drwBitmapExporter();
	void SetScene( Scene * scene ) { m_scene = scene; }
	void SetFilename( QString filename ) { m_filename = filename; }
	
	virtual void run();
	
signals:
	
	void WritingFrame( int frame );
	
	
protected:
	
	Scene * m_scene;
	QString m_filename;
	
};

#endif