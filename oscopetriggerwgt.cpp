#include "oscopetriggerwgt.h"
#include "ui_oscopetriggerwgt.h"
#include "regtypes.h"
#include <QPair>



OScopeTriggerWgt::OScopeTriggerWgt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OScopeTriggerWgt)
{
    ui->setupUi(this);

    populateTriggerTypes();

    connect(ui->cbTrigEnabled, QCheckBox::stateChanged, this, &OScopeTriggerWgt::trigEnabled_stateChanged);
    connect(ui->cbTrigType, static_cast<void (QComboBox::*)(int)>(QComboBox::currentIndexChanged), this, &OScopeTriggerWgt::trigType_currentIndexChanged);
    connect(ui->cbTrigChannel, static_cast<void (QComboBox::*)(int)>(QComboBox::currentIndexChanged), this, &OScopeTriggerWgt::trigChannel_currentIndexChanged);
    connect(ui->dsbTrigValue, static_cast<void (QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged), this, &OScopeTriggerWgt::trigValue_valueChanged);
}

OScopeTriggerWgt::~OScopeTriggerWgt()
{
    delete ui;
}

bool OScopeTriggerWgt::triggerEnabled() const
{
    return ui->cbTrigEnabled->isChecked();
}

void OScopeTriggerWgt::setTriggerEnabled(bool newEnabled)
{
    ui->cbTrigEnabled->setChecked(newEnabled);
}

SDOScope::TriggerType OScopeTriggerWgt::triggerType() const
{
    bool ok = false;
    auto res =  static_cast<SDOScope::TriggerType>(ui->cbTrigType->currentData().toInt(&ok));
    if(ok) return res;
    return SDOScope::TRIGGER_RISING;
}

void OScopeTriggerWgt::setTriggerType(SDOScope::TriggerType newType)
{
    int type_index = ui->cbTrigType->findData(static_cast<int>(newType));
    if(type_index != -1) ui->cbTrigType->setCurrentIndex(type_index);
}

qreal OScopeTriggerWgt::triggerValue() const
{
    return ui->dsbTrigValue->value();
}

void OScopeTriggerWgt::setTriggerValue(qreal newValue)
{
    ui->dsbTrigValue->setValue(newValue);
}

uint OScopeTriggerWgt::triggerChannel() const
{
    bool ok = false;
    auto res =  static_cast<uint>(ui->cbTrigChannel->currentData().toUInt(&ok));
    if(ok) return res;
    return 0;
}

void OScopeTriggerWgt::setTriggerChannel(uint newChannel)
{
    int ch_index = ui->cbTrigChannel->findData(static_cast<uint>(newChannel));
    if(ch_index != -1) ui->cbTrigChannel->setCurrentIndex(ch_index);
}

void OScopeTriggerWgt::clearTriggerChannels()
{
    ui->cbTrigChannel->clear();
}

void OScopeTriggerWgt::addTriggerChannel(QString newChannelName, uint newChannel)
{
    ui->cbTrigChannel->addItem(newChannelName, newChannel);
}

qreal OScopeTriggerWgt::triggerValueMinimum() const
{
    return ui->dsbTrigValue->minimum();
}

void OScopeTriggerWgt::setTriggerValueMinimum(qreal newMin)
{
    ui->dsbTrigValue->setMinimum(newMin);
}

qreal OScopeTriggerWgt::triggerValueMaximum() const
{
    return ui->dsbTrigValue->maximum();
}

void OScopeTriggerWgt::setTriggerValueMaximum(qreal newMax)
{
    ui->dsbTrigValue->setMaximum(newMax);
}

int OScopeTriggerWgt::triggerValueDecimals() const
{
    return ui->dsbTrigValue->decimals();
}

void OScopeTriggerWgt::setTriggerValueDecimals(int newDecimals)
{
    ui->dsbTrigValue->setDecimals(newDecimals);
}

qreal OScopeTriggerWgt::triggerValueSingleStep() const
{
    return ui->dsbTrigValue->singleStep();
}

void OScopeTriggerWgt::setTriggerValueSingleStep(qreal newSingleStep)
{
    ui->dsbTrigValue->setSingleStep(newSingleStep);
}

void OScopeTriggerWgt::trigEnabled_stateChanged(int newState)
{
    switch(newState){
    default:
        break;
    case Qt::Checked:
        emit triggerEnabledChanged(true);
        break;
    case Qt::Unchecked:
        emit triggerEnabledChanged(false);
        break;
    }
}

void OScopeTriggerWgt::trigType_currentIndexChanged(int newIndex)
{
    if(newIndex < 0) return;
    bool ok = false;
    auto trig_type = ui->cbTrigType->currentData().toInt(&ok);
    if(ok) emit triggerTypeChanged(trig_type);
}

void OScopeTriggerWgt::trigChannel_currentIndexChanged(int newIndex)
{
    if(newIndex < 0) return;
    bool ok = false;
    auto trig_ch = ui->cbTrigChannel->currentData().toUInt(&ok);
    if(ok) emit triggerChannelChanged(trig_ch);
}

void OScopeTriggerWgt::trigValue_valueChanged(qreal newValue)
{
    emit triggerValueChanged(newValue);
}

void OScopeTriggerWgt::populateTriggerTypes()
{
    QList<QPair<QString, SDOScope::TriggerType>> trig_types;

    trig_types << qMakePair(tr("_↑‾|_"), SDOScope::TRIGGER_RISING)
               << qMakePair(tr("‾↓_|‾"), SDOScope::TRIGGER_FALLING);

    ui->cbTrigType->clear();

    for(auto& trig_type: trig_types){
        ui->cbTrigType->addItem(trig_type.first, static_cast<int>(trig_type.second));
    }
}
