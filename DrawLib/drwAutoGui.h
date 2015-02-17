#ifndef __drwAutoGui_h_
#define __drwAutoGui_h_

#include <QWidget>

class QSlider;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class GuiParam;

class drwAutoGui : public QWidget
{
    Q_OBJECT

public:
    explicit drwAutoGui( QObject * target, QWidget * parent = 0 );
    void AddIntSliderParam( QString paramName, int minVal, int maxVal );
    void AddDoubleSliderParam( QString paramName, double minVal, double maxVal );
    void AddChildLayout( QHBoxLayout * childLayout );
    QObject * GetTarget() { return m_target; }

public slots:
    void TargetModified();

protected:
    QList<GuiParam*> m_params;
    QVBoxLayout * m_guiLayout;
    QObject * m_target;
};

class GuiParam : public QObject
{
    Q_OBJECT
public:
    GuiParam( drwAutoGui * parent, QString name ) : QObject( parent ), m_name(name), m_autoGui(parent) {}
    virtual void Update() {}
protected:
    QString m_name;
    drwAutoGui * m_autoGui;
};

class IntSliderGuiParam : public GuiParam
{
    Q_OBJECT
public:
    IntSliderGuiParam( drwAutoGui * parent, QString name, int minVal, int maxVal );
private slots:
    void OnSliderValueChanged( int );
protected:
    const char * GetValueName();
    virtual void Update();
    QSlider * m_slider;
    QLineEdit * m_lineEdit;
};

class DoubleSliderGuiParam : public GuiParam
{
    Q_OBJECT
public:
    DoubleSliderGuiParam( drwAutoGui * parent, QString name, double minVal, double maxVal );
private slots:
    void OnSliderValueChanged( int );
protected:
    const char * GetValueName();
    double SliderValueToParamValue(int paramValue);
    int ParamValueToSliderValue( double paramValue );
    virtual void Update();
    QSlider * m_slider;
    QLineEdit * m_lineEdit;
    double m_range[2];
};


#endif
