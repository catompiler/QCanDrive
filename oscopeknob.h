#ifndef OSCOPEKNOB_H
#define OSCOPEKNOB_H

#include <QwtKnob>

class QMouseEvent;

class OScopeKnob : public QwtKnob
{
    Q_OBJECT
public:
    OScopeKnob(QWidget* parent = nullptr);
    ~OScopeKnob();

signals:
    void sliderDoubleClicked(int buttons, int modifiers);

    // QWidget interface
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
};

#endif // OSCOPEKNOB_H
