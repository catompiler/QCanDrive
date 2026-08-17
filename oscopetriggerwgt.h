#ifndef OSCOPETRIGGERWGT_H
#define OSCOPETRIGGERWGT_H

#include <QWidget>
#include "sdoscope.h"
#include <stdint.h>
#include "regtypes.h"


class QTimer;


namespace Ui {
class OScopeTriggerWgt;
}

class OScopeTriggerWgt : public QWidget
{
    Q_OBJECT

public:

    static constexpr int VALUE_CHANGED_DELAY_MS = 500;

    explicit OScopeTriggerWgt(QWidget *parent = nullptr);
    ~OScopeTriggerWgt();

    bool triggerEnabled() const;
    void setTriggerEnabled(bool newEnabled);

    SDOScope::TriggerType triggerType() const;
    void setTriggerType(SDOScope::TriggerType newType);

    DataType triggerDataType() const;
    void setTriggerDataType(DataType newDataType);

    qreal triggerValue() const;
    void setTriggerValue(qreal newValue);

    int32_t triggerDataValue() const;
    void setTriggerDataValue(int32_t newValue);

    uint triggerChannel() const;
    void setTriggerChannel(uint newChannel);
    void clearTriggerChannels();
    void addTriggerChannel(QString newChannelName, uint newChannel);

signals:
    void triggerEnabledChanged(bool newEnabled);
    void triggerTypeChanged(int newType /* SDOScope::TriggerType */);
    void triggerValueChanged(qreal newValue);
    void triggerDataValueChanged(int newValue);
    void triggerChannelChanged(uint newChannel);

private slots:
    void trigEnabled_stateChanged(int newState);
    void trigType_currentIndexChanged(int newIndex);
    void trigChannel_currentIndexChanged(int newIndex);
    void dataType_currentIndexChanged(int newIndex);
    void trigValue_valueChanged(qreal newValue);
    void valueChangedTmr_timeout();

private:
    Ui::OScopeTriggerWgt *ui;
    QTimer* m_valueChangedTmr;

    void populateTriggerTypes();
    void populateDataTypes();
    void updateValueUi();
};

#endif // OSCOPETRIGGERWGT_H
