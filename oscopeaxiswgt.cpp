#include "oscopeaxiswgt.h"
#include "ui_oscopeaxiswgt.h"
#include <QwtRoundScaleDraw>
#include <math.h>
#include <algorithm>


OScopeAxisWgt::OScopeAxisWgt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OScopeAxisWgt)
{
    ui->setupUi(this);

    QwtKnob* knob = ui->valOffset;
    knob->setKnobStyle( QwtKnob::Styled );
    knob->setMarkerStyle( QwtKnob::Nub );
    knob->setWrapping( false );
    knob->setNumTurns( OFFSET_TURNS );
    knob->setScale( -OFFSET_STEPS, OFFSET_STEPS );
    knob->setTotalSteps( OFFSET_STEPS * 2 );
    knob->setValue(0.0);
    //knob->setScaleStepSize( 0.0 );
    if(QwtRoundScaleDraw* scaleDraw = knob->scaleDraw(); scaleDraw != nullptr){
        scaleDraw->enableComponent(QwtAbstractScaleDraw::Ticks, false);
        scaleDraw->enableComponent(QwtAbstractScaleDraw::Labels, false);
        scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }

    knob = ui->valScale;
    knob->setKnobStyle( QwtKnob::Styled );
    knob->setMarkerStyle( QwtKnob::Nub );
    knob->setWrapping( false );
    knob->setNumTurns( SCALE_TURNS );
    knob->setScale( -SCALE_STEPS, SCALE_STEPS );
    knob->setTotalSteps( SCALE_STEPS * 2 );
    knob->setValue(0.0);
    //knob->setScaleStepSize( 0.0 );
    if(QwtRoundScaleDraw* scaleDraw = knob->scaleDraw(); scaleDraw != nullptr){
        scaleDraw->enableComponent(QwtAbstractScaleDraw::Ticks, false);
        scaleDraw->enableComponent(QwtAbstractScaleDraw::Labels, false);
        scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }

    connect(ui->valOffset, &QwtKnob::valueChanged, this, &OScopeAxisWgt::offset_valueChanged);
    connect(ui->valScale, &QwtKnob::valueChanged, this, &OScopeAxisWgt::scale_valueChanged);

    updateOffsetDispVal();
    updateScaleDispVal();
}

OScopeAxisWgt::~OScopeAxisWgt()
{
    delete ui;
}

QString OScopeAxisWgt::scaleTitle() const
{
    return m_scaleTitle;
}

void OScopeAxisWgt::setScaleTitle(const QString& newScaleTitle)
{
    m_scaleTitle = newScaleTitle;
}

QString OScopeAxisWgt::offsetTitle() const
{
    return m_offsetTitle;
}

void OScopeAxisWgt::setOffsetTitle(const QString& newOffsetTitle)
{
    m_offsetTitle = newOffsetTitle;
}

QString OScopeAxisWgt::unit() const
{
    return m_unit;
}

void OScopeAxisWgt::setUnit(const QString& newUnit)
{
    m_unit = newUnit;
}

qreal OScopeAxisWgt::scaleMin() const
{
    return m_scaleMin;
}

void OScopeAxisWgt::setScaleMin(qreal newScaleMin)
{
    m_scaleMin = newScaleMin;
}

qreal OScopeAxisWgt::scaleMax() const
{
    return m_scaleMax;
}

void OScopeAxisWgt::setScaleMax(qreal newScaleMax)
{
    m_scaleMax = newScaleMax;
}

qreal OScopeAxisWgt::scale() const
{
    int value_index = ui->valScale->value();
    qreal value = valueFromScaleIndex(value_index);

    return value;
}

void OScopeAxisWgt::setScale(qreal newScale)
{
    m_scale = newScale;
}

qreal OScopeAxisWgt::offsetMin() const
{
    return m_offsetMin;
}

void OScopeAxisWgt::setOffsetMin(qreal newOffsetMin)
{
    m_offsetMin = newOffsetMin;
}

qreal OScopeAxisWgt::offsetMax() const
{
    return m_offsetMax;
}

void OScopeAxisWgt::setOffsetMax(qreal newOffsetMax)
{
    m_offsetMax = newOffsetMax;
}

qreal OScopeAxisWgt::offset() const
{
    int value_index = ui->valOffset->value();
    qreal value = valueFromOffsetIndex(value_index);

    return value;
}

void OScopeAxisWgt::setOffset(qreal newOffset)
{
    m_offset = newOffset;
}

void OScopeAxisWgt::scale_valueChanged(double value)
{
    Q_UNUSED(value);

    updateScaleDispVal();
    updateOffsetDispVal();

    emit scaleChanged(scale());
    emit offsetChanged(offset());
}

void OScopeAxisWgt::offset_valueChanged(double value)
{
    Q_UNUSED(value);

    updateOffsetDispVal();

    emit offsetChanged(offset());
}

QPair<qreal, int> OScopeAxisWgt::valueScaleBase(qreal val, qreal scaleBase) const
{
    if(val <= 0 || scaleBase <= 0) return qMakePair(0.0, 0);

    qreal expBase = floor(log10(val) /  log10(scaleBase));
    qreal valExpBase = val / pow(scaleBase, expBase);

    return qMakePair(valExpBase, static_cast<int>(expBase));
}

qreal OScopeAxisWgt::valueFromScaleIndex(int idx) const
{
    int power = (idx >= 0) ? (idx / SCALE_LEN) : ((idx + 1) / SCALE_LEN - 1);
    int index = (idx >= 0) ? (idx % SCALE_LEN) : (SCALE_LEN - 1 - (abs((idx + 1)) % SCALE_LEN));

    return pow(10.0, power) * SCALE_VALUES[index];
}

qreal OScopeAxisWgt::valueFromOffsetIndex(int idx) const
{
    return scale() * idx * OFFSET_K;
}

int OScopeAxisWgt::scaleIndexFromValue(qreal scaleVal) const
{
    if(scaleVal <= 0.0) return 0;

    qreal valE = floor(log10(scaleVal)); // Экспонента.
    qreal valM = scaleVal / pow(10.0, valE); // Мантисса.

    int min_index = 0;
    qreal min_err = fabs(valM - SCALE_VALUES[min_index]);

    for(int i = 1; i < SCALE_LEN; i ++){
        qreal err = fabs(valM - SCALE_VALUES[i]);
        // <= для выбора следующего масштаба для среднего значения.
        if(err <= min_err){
            min_err = err;
            min_index = i;
        }
    }

    return static_cast<int>(valE) * SCALE_LEN + min_index;
}

int OScopeAxisWgt::offsetIndexFromValue(qreal offsetVal) const
{
    return round(offsetVal / (scale() * OFFSET_K));
}

void OScopeAxisWgt::updateScaleDispVal()
{
    qreal value = scale();

    ui->lblScaleVal->setText(QString::number(value));
}

void OScopeAxisWgt::updateOffsetDispVal()
{
    qreal value = offset();

    ui->lblOffsetVal->setText(QString::number(value));
}
