#include "oscopeknob.h"
#include <QMouseEvent>


OScopeKnob::OScopeKnob(QWidget* parent)
    :QwtKnob(parent)
{
}

OScopeKnob::~OScopeKnob()
{

}

void OScopeKnob::mouseDoubleClickEvent(QMouseEvent* event)
{
    QwtKnob::mouseDoubleClickEvent(event);

    emit sliderDoubleClicked(static_cast<int>(event->buttons()), static_cast<int>(event->modifiers()));
}
