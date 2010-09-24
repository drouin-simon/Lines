#ifndef __drwDrawingWidget_h_
#define __drwDrawingWidget_h_

#include <QGLWidget>
#include "Camera.h"
#include "macros.h"
#include "Scene.h"
#include "drwCommand.h"

class drwWidgetObserver;
class drwDrawingWidgetInteractor;
class Node;
class PlaybackControler;
class drwDisplaySettings;
class drwDrawableTexture;

class drwDrawingWidget : public QGLWidget
{
	Q_OBJECT
  
public:
		
	drwDrawingWidget( QWidget * parent=0 );
	~drwDrawingWidget();
	
	Node * Pick( int x, int y );
	void SetCurrentScene( Scene * cur );
	SetMacro( Observer, drwWidgetObserver* );
	void SetControler( PlaybackControler * controler );
	GetMacro( Controler, PlaybackControler* );
	GetMacro( DisplaySettings, drwDisplaySettings* );
	drwDrawableTexture * GetWorkTexture() { return m_workTexture; }
	Camera & GetCamera() { return theCamera; }
	
	drwCommand::s_ptr CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e );
	drwCommand::s_ptr CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e );
	void WindowToWorld( double xWin, double yWin, double & xWorld, double & yWorld );
	
public slots:
	
	void RequestRedraw();
    void CurrentFrameChanged();
	void PlaybackStartStop( bool isStart );
	void DisplaySettingsModified();
		
protected:
		
	virtual void initializeGL();
	virtual void resizeGL( int width, int height );
	virtual void paintGL();
	void DrawAllFramesHue();
	void DrawAllFramesRedGreen();
	void DrawFrame();
	void DisplayCounter();

	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void tabletEvent ( QTabletEvent * event );
	
	// Manage timer that generates updateGL in playback mode
	int m_timerId;
	
	void enterEvent( QEvent * );
	void leaveEvent( QEvent * );
	
	// Handling of Drag and Drop
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);
	
	// stuff to do for all events
	bool event ( QEvent * event );
  
private:
	
	Camera theCamera;
	drwDrawingWidgetInteractor * m_interactor;
	drwWidgetObserver * Observer;
	Scene				* CurrentScene;
	PlaybackControler	* Controler;
	drwDisplaySettings	* DisplaySettings;
	drwDrawableTexture * m_workTexture;
	
	bool HasDrawn;
	bool m_needUpdateGL;
	
}; 

#endif
