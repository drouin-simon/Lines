#include "drwDrawingWidgetInteractor.h"
#include "drwDrawingWidget.h"

drwDrawingWidgetInteractor::drwDrawingWidgetInteractor( drwDrawingWidget * widget )
: currentMouseX(0)
, currentMouseY(0)
, lastMouseX(0)
, lastMouseY(0)
, mouseMoving(false)
, isScaling(false)
, isPaning(false)
, m_widget( widget )
{
}

drwDrawingWidgetInteractor::~drwDrawingWidgetInteractor()
{
}


bool drwDrawingWidgetInteractor::mousePressEvent( QMouseEvent *e )
{
	if ( e->button() == Qt::LeftButton )
	{
		return Press( e->x(), e->y(), e->modifiers() );
	}
	return false;
}

bool drwDrawingWidgetInteractor::mouseReleaseEvent( QMouseEvent *e )
{
	if ( e->button() == Qt::LeftButton )
	{
		return Release();
	}
	return false;
}

bool drwDrawingWidgetInteractor::mouseMoveEvent( QMouseEvent *e )
{
	return Move( e->x(), e->y() );
}

bool drwDrawingWidgetInteractor::tabletEvent ( QTabletEvent * e )
{
	if( e->type() == QEvent::TabletPress )
		return Press( e->x(), e->y(), e->modifiers() );
	if( e->type() == QEvent::TabletRelease )
		return Release();
	if( e->type() == QEvent::TabletMove )
		return Move( e->x(), e->y() );
	
	return false;
}

bool drwDrawingWidgetInteractor::Press( int x, int y, Qt::KeyboardModifiers modifiers )
{
	mouseMoving = false;
	currentMouseX = x;
	currentMouseY = y;
	
	if( modifiers & Qt::ShiftModifier )
	{
		isPaning = true;
		mouseMoving = true;
	}
	else if ( modifiers & Qt::ControlModifier )
	{
		isScaling = true;
		mouseMoving = true;
	}
	return mouseMoving;
}

bool drwDrawingWidgetInteractor::Release()
{
	if( isPaning || isScaling )
	{
		// cancel all operation on view
		mouseMoving = false;
		isPaning    = false;
		isScaling   = false;
		return true;
	}
	return false;
}

bool drwDrawingWidgetInteractor::Move( int x, int y )
{
	lastMouseX = currentMouseX;
	lastMouseY = currentMouseY;
	currentMouseX = x;
	currentMouseY = y;
	
	bool selfInducedRedraw = false;
	Camera & cam = m_widget->GetCamera();
	if( isScaling )
	{
		cam.Zoom( 1.0 + (double)(currentMouseY - lastMouseY) / (double)100.0 );
        m_widget->ShowFullFrame( false );
		selfInducedRedraw = true;
	}
	else if( isPaning )
	{
		double xPercent = -1.0 * (double)( currentMouseX - lastMouseX ) / (double)( cam.GetWindowWidth() );
		double yPercent = (double)( currentMouseY - lastMouseY ) / (double)( cam.GetWindowHeight() );
		cam.Pan( xPercent, yPercent );
        m_widget->ShowFullFrame( false );
		selfInducedRedraw = true;
	}
	if( selfInducedRedraw )
	{
		m_widget->RequestRedraw();
		return true;
	}
	return false;
}
