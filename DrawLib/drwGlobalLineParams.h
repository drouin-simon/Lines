#ifndef __drwGlobalLineParams_h_
#define __drwGlobalLineParams_h_

#include <QObject>

class drwDrawingWidget;

class drwGlobalLineParams : public QObject
{
    Q_OBJECT
    Q_PROPERTY( double PixMargin READ GetPixMargin WRITE SetPixMargin );
    Q_PROPERTY( double PixDampWidth READ GetPixDampWidth WRITE SetPixDampWidth );
    Q_PROPERTY( double SigmaLarge READ GetSigmaLarge WRITE SetSigmaLarge );
    Q_PROPERTY( double SigmaSmall READ GetSigmaSmall WRITE SetSigmaSmall );

public:
    
    void SetDrawingWidget( drwDrawingWidget * w ) { m_drawingWidget = w; }
    
    void SetPixMargin( double pixMarging );
    double GetPixMargin();
    void SetPixDampWidth( double pixDampWidth );
    double GetPixDampWidth();
    void SetSigmaLarge( double sigmaLarge );
    double GetSigmaLarge();
    void SetSigmaSmall( double sigmaSmall );
    double GetSigmaSmall();

    explicit drwGlobalLineParams( QObject * parent = 0 );
    QWidget * CreateGui();
    
signals:
    
    void Modified();
    
protected:
    
    void MarkModified();
    
    drwDrawingWidget * m_drawingWidget;

};

#endif
