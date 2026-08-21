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
    populateDataTypes();
    updateValueUi();

    connect(ui->cbTrigEnabled, QCheckBox::stateChanged, this, &OScopeTriggerWgt::trigEnabled_stateChanged);
    connect(ui->cbTrigType, static_cast<void (QComboBox::*)(int)>(QComboBox::currentIndexChanged), this, &OScopeTriggerWgt::trigType_currentIndexChanged);
    connect(ui->cbTrigChannel, static_cast<void (QComboBox::*)(int)>(QComboBox::currentIndexChanged), this, &OScopeTriggerWgt::trigChannel_currentIndexChanged);
    connect(ui->cbDataType, static_cast<void (QComboBox::*)(int)>(QComboBox::currentIndexChanged), this, &OScopeTriggerWgt::dataType_currentIndexChanged);
    //connect(ui->dsbTrigValue, static_cast<void (QDoubleSpinBox::*)(qreal)>(&QDoubleSpinBox::valueChanged), this, &OScopeTriggerWgt::triggerValueChanged);
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

DataType OScopeTriggerWgt::triggerDataType() const
{
    bool ok = false;
    auto res =  static_cast<DataType>(ui->cbDataType->currentData().toUInt(&ok));
    if(ok) return res;
    return DataType::I32;
}

void OScopeTriggerWgt::setTriggerDataType(DataType newDataType)
{
    int type_index = ui->cbDataType->findData(static_cast<uint>(newDataType));
    if(type_index != -1) ui->cbDataType->setCurrentIndex(type_index);

    updateValueUi();
}

qreal OScopeTriggerWgt::triggerBaseValue() const
{
    return ui->dsbBaseValue->value();
}

void OScopeTriggerWgt::setTriggerBaseValue(qreal newValue)
{
    ui->dsbBaseValue->setValue(newValue);

    updateValueUi();
}

qreal OScopeTriggerWgt::triggerValue() const
{
    return ui->dsbTrigValue->value();
}

void OScopeTriggerWgt::setTriggerValue(qreal newValue)
{
    ui->dsbTrigValue->setValue(newValue);
}

int32_t OScopeTriggerWgt::triggerDataValue() const
{
    bool ok = false;
    DataType dataType = static_cast<DataType>(ui->cbDataType->currentData().toUInt(&ok));
    if(!ok) return 0;

    qreal val = ui->dsbTrigValue->value();
    qreal baseVal = ui->dsbBaseValue->value();

    qreal valPU = val / baseVal;

    qreal res_val = 0.0;

    switch(dataType){
    default:
        break;
    case DataType::I32:
    case DataType::I16:
    case DataType::I8:
    case DataType::U32:
    case DataType::U16:
    case DataType::U8:
        res_val = valPU;
        break;
    case DataType::IQ24:
        res_val = valPU * (1L << 24);
        break;
    case DataType::IQ15:
        res_val = valPU * (1L << 15);
        break;
    case DataType::IQ7:
        res_val =  valPU * (1L << 7);
        break;
    }

    return res_val;
}

void OScopeTriggerWgt::setTriggerDataValue(int32_t newValue)
{
    bool ok = false;
    DataType dataType = static_cast<DataType>(ui->cbDataType->currentData().toUInt(&ok));
    if(!ok) return;

    qreal baseVal = ui->dsbBaseValue->value();

    qreal valPU = 0.0;

    switch(dataType){
    default:
        break;
    case DataType::I32:
    case DataType::I16:
    case DataType::I8:
    case DataType::U32:
    case DataType::U16:
    case DataType::U8:
        valPU = newValue;
        break;
    case DataType::IQ24:
        valPU = static_cast<qreal>(newValue) / (1L << 24);
        break;
    case DataType::IQ15:
        valPU = static_cast<qreal>(newValue) / (1L << 15);
        break;
    case DataType::IQ7:
        valPU = static_cast<qreal>(newValue) / (1L << 7);
        break;
    }

    qreal val = valPU * baseVal;

    ui->dsbTrigValue->setValue(val);
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

void OScopeTriggerWgt::dataType_currentIndexChanged(int newIndex)
{
    if(newIndex == -1) return;

    updateValueUi();
}

void OScopeTriggerWgt::trigValue_valueChanged(qreal newValue)
{
    Q_UNUSED(newValue);

    emit triggerValueChanged(triggerValue());
    emit triggerDataValueChanged(triggerDataValue());
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

void OScopeTriggerWgt::populateDataTypes()
{
    auto dataTypes = RegTypes::dataTypes();

    ui->cbDataType->clear();

    for(auto& dataType: dataTypes){
        if(!RegTypes::isNumeric(dataType)) continue;

        ui->cbDataType->addItem(RegTypes::dataTypeStr(dataType), static_cast<uint>(dataType));
    }
}

void OScopeTriggerWgt::updateValueUi()
{
    bool ok = false;
    DataType dataType = static_cast<DataType>(ui->cbDataType->currentData().toUInt(&ok));
    if(!ok) return;

    qreal baseVal = ui->dsbBaseValue->value();

    //int32_t dataValue = triggerDataValue();

    switch(dataType){
    default:
        return;
    case DataType::I32:
        ui->dsbTrigValue->setMinimum(baseVal * INT32_MIN);
        ui->dsbTrigValue->setMaximum(baseVal * INT32_MAX);
        ui->dsbTrigValue->setSingleStep(1);
        ui->dsbTrigValue->setDecimals(0);
        break;
    case DataType::I16:
        ui->dsbTrigValue->setMinimum(baseVal * INT16_MIN);
        ui->dsbTrigValue->setMaximum(baseVal * INT16_MAX);
        ui->dsbTrigValue->setSingleStep(1);
        ui->dsbTrigValue->setDecimals(0);
        break;
    case DataType::I8:
        ui->dsbTrigValue->setMinimum(baseVal * INT8_MIN);
        ui->dsbTrigValue->setMaximum(baseVal * INT8_MAX);
        ui->dsbTrigValue->setSingleStep(1);
        ui->dsbTrigValue->setDecimals(0);
        break;
    case DataType::U32:
        ui->dsbTrigValue->setMinimum(0);
        ui->dsbTrigValue->setMaximum(baseVal * UINT32_MAX);
        ui->dsbTrigValue->setSingleStep(1);
        ui->dsbTrigValue->setDecimals(0);
        break;
    case DataType::U16:
        ui->dsbTrigValue->setMinimum(0);
        ui->dsbTrigValue->setMaximum(baseVal * UINT16_MAX);
        ui->dsbTrigValue->setSingleStep(1);
        ui->dsbTrigValue->setDecimals(0);
        break;
    case DataType::U8:
        ui->dsbTrigValue->setMinimum(0);
        ui->dsbTrigValue->setMaximum(baseVal * UINT8_MAX);
        ui->dsbTrigValue->setSingleStep(1);
        ui->dsbTrigValue->setDecimals(0);
        break;
    case DataType::IQ24:
        ui->dsbTrigValue->setMinimum(baseVal * static_cast<qreal>(INT32_MIN) / (1L << 24));
        ui->dsbTrigValue->setMaximum(baseVal * static_cast<qreal>(INT32_MAX) / (1L << 24));
        ui->dsbTrigValue->setSingleStep(0.1);
        ui->dsbTrigValue->setDecimals(7);
        break;
    case DataType::IQ15:
        ui->dsbTrigValue->setMinimum(baseVal * static_cast<qreal>(INT32_MIN) / (1L << 15));
        ui->dsbTrigValue->setMaximum(baseVal * static_cast<qreal>(INT32_MAX) / (1L << 15));
        ui->dsbTrigValue->setSingleStep(0.1);
        ui->dsbTrigValue->setDecimals(4);
        break;
    case DataType::IQ7:
        ui->dsbTrigValue->setMinimum(baseVal * static_cast<qreal>(INT32_MIN) / (1L << 7));
        ui->dsbTrigValue->setMaximum(baseVal * static_cast<qreal>(INT32_MAX) / (1L << 7));
        ui->dsbTrigValue->setSingleStep(0.1);
        ui->dsbTrigValue->setDecimals(2);
        break;
    }

    //setTriggerDataValue(dataValue);
}
