#ifndef OSCOPETRIGGERWGT_H
#define OSCOPETRIGGERWGT_H

#include <QWidget>
#include "sdoscope.h"
#include <stdint.h>
#include "regtypes.h"


namespace Ui {
class OScopeTriggerWgt;
}

class OScopeTriggerWgt : public QWidget
{
    Q_OBJECT

public:
    explicit OScopeTriggerWgt(QWidget *parent = nullptr);
    ~OScopeTriggerWgt();

    bool triggerEnabled() const;
    void setTriggerEnabled(bool newEnabled);

    SDOScope::TriggerType triggerType() const;
    void setTriggerType(SDOScope::TriggerType newType);

    qreal triggerValue() const;
    void setTriggerValue(qreal newValue);

    uint triggerChannel() const;
    void setTriggerChannel(uint newChannel);
    void clearTriggerChannels();
    void addTriggerChannel(QString newChannelName, uint newChannel);

    qreal triggerValueMinimum() const;
    void setTriggerValueMinimum(qreal newMin);

    qreal triggerValueMaximum() const;
    void setTriggerValueMaximum(qreal newMax);

    int triggerValueDecimals() const;
    void setTriggerValueDecimals(int newDecimals);

    qreal triggerValueSingleStep() const;
    void setTriggerValueSingleStep(qreal newSingleStep);

signals:
    void triggerEnabledChanged(bool newEnabled);
    void triggerTypeChanged(int newType /* SDOScope::TriggerType */);
    void triggerValueChanged(qreal newValue);
    void triggerChannelChanged(uint newChannel);

private slots:
    void trigEnabled_stateChanged(Qt::CheckState newState);
    void trigType_currentIndexChanged(int newIndex);
    void trigChannel_currentIndexChanged(int newIndex);
    void trigValue_valueChanged(qreal newValue);

private:
    Ui::OScopeTriggerWgt *ui;

    void populateTriggerTypes();
};

#endif // OSCOPETRIGGERWGT_H
