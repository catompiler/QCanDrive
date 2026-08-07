#include "oscopeaxiswgt.h"
#include "ui_oscopeaxiswgt.h"
#include <QwtRoundScaleDraw>
#include <math.h>
#include <algorithm>
#include <QDebug>


OScopeAxisWgt::OScopeAxisWgt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OScopeAxisWgt)
{
    ui->setupUi(this);

    QwtKnob* knob = ui->valOffset;
    knob->setKnobStyle( QwtKnob::Styled );
    knob->setMarkerStyle( QwtKnob::Nub );
    knob->setWrapping( false );
    knob->setTotalAngle(360.0);
    knob->setNumTurns( 10 );
    knob->setScale( -10, 10 );
    knob->setTotalSteps( 20 );
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
    knob->setTotalAngle(360.0);
    knob->setNumTurns( 4 );
    knob->setScale( -10, 10 );
    knob->setTotalSteps( 20 );
    knob->setValue(0.0);
    //knob->setScaleStepSize( 0.0 );
    if(QwtRoundScaleDraw* scaleDraw = knob->scaleDraw(); scaleDraw != nullptr){
        scaleDraw->enableComponent(QwtAbstractScaleDraw::Ticks, false);
        scaleDraw->enableComponent(QwtAbstractScaleDraw::Labels, false);
        scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }

    updateOffsetRange();

    updateOffsetDispVal();
    updateScaleDispVal();

    connect(ui->valOffset, &QwtKnob::valueChanged, this, &OScopeAxisWgt::offset_valueChanged);
    connect(ui->valScale, &QwtKnob::valueChanged, this, &OScopeAxisWgt::scale_valueChanged);
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

QStringList OScopeAxisWgt::unitUpPrefixes() const
{
    return m_unitUpPrefixes;
}

QStringList OScopeAxisWgt::unitDownPrefixes() const
{
    return m_unitDownPrefixes;
}

void OScopeAxisWgt::setUnit(const QString& newUnit, const QStringList& upPrefixes, const QStringList& downPrefixes)
{
    m_unit = newUnit;
    m_unitUpPrefixes = upPrefixes;
    m_unitDownPrefixes = downPrefixes;

    updateScaleDispVal();
    updateOffsetDispVal();
}

qreal OScopeAxisWgt::scaleMin() const
{
    return valueFromScaleIndex(ui->valScale->lowerBound());
}

void OScopeAxisWgt::setScaleMin(qreal newScaleMin)
{
    int minIndex = scaleIndexFromValue(newScaleMin);
    int maxIndex = static_cast<int>(ui->valScale->upperBound());

    ui->valScale->setLowerBound(minIndex);
    ui->valScale->setTotalSteps(abs(minIndex) + abs(maxIndex));
}

qreal OScopeAxisWgt::scaleMax() const
{
    return valueFromScaleIndex(ui->valScale->upperBound());
}

void OScopeAxisWgt::setScaleMax(qreal newScaleMax)
{
    int minIndex = static_cast<int>(ui->valScale->lowerBound());
    int maxIndex = scaleIndexFromValue(newScaleMax);

    ui->valScale->setUpperBound(maxIndex);
    ui->valScale->setTotalSteps(abs(minIndex) + abs(maxIndex));
}

qreal OScopeAxisWgt::scale() const
{
    int value_index = ui->valScale->value();
    qreal value = valueFromScaleIndex(value_index);

    return value;
}

void OScopeAxisWgt::setScale(qreal newScale)
{
    ui->valScale->setValue(scaleIndexFromValue(newScale));
}

int OScopeAxisWgt::scaleTurns() const
{
    return ui->valScale->numTurns();
}

void OScopeAxisWgt::setScaleTurns(int newScaleTurns)
{
    ui->valScale->setNumTurns(newScaleTurns);
}

qreal OScopeAxisWgt::offset() const
{
    return valueFromOffsetIndex(ui->valOffset->value());
}

void OScopeAxisWgt::setOffset(qreal newOffset)
{
    ui->valOffset->setValue(offsetIndexFromValue(newOffset));
}

void OScopeAxisWgt::scale_valueChanged(double value)
{
    Q_UNUSED(value);

    //updateOffsetRange();

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

QPair<QString, int> OScopeAxisWgt::getUnitPrefix(int expVal) const
{
    if(expVal == 0) return qMakePair(QString(), 0);

    const QStringList& prefixes = (expVal >= 0) ? m_unitUpPrefixes : m_unitDownPrefixes;

    if(prefixes.empty()) return qMakePair(QString(), 0);

    int index = (expVal > 0) ? expVal - 1 : -(expVal + 1);

    if(index >= prefixes.size()) index = prefixes.size() - 1;

    return qMakePair(prefixes.at(index), (expVal >= 0) ? index + 1 : -index - 1);
}

QString OScopeAxisWgt::getDispVal(qreal dispVal) const
{
    int expVal = 0;

    if(dispVal > 0.0){
        expVal = static_cast<int>(floor(log10( dispVal) / 3.0)); //log1000
    }else if(dispVal < 0.0){
        expVal = static_cast<int>(floor(log10(-dispVal) / 3.0)); //log1000
    }

    auto prefixPair = getUnitPrefix(expVal);

    //qDebug() << expVal << prefixPair;

    QString prefix = prefixPair.first;
    qreal value = dispVal / pow(1000, prefixPair.second);

    return QString("%1 %2%3")
        .arg(QString::number(value))
        .arg(prefix)
        .arg(m_unit);
}

void OScopeAxisWgt::updateScaleDispVal()
{
    ui->lblScaleVal->setText(getDispVal(scale()));
}

void OScopeAxisWgt::updateOffsetDispVal()
{
    ui->lblOffsetVal->setText(getDispVal(offset()));
}

void OScopeAxisWgt::updateOffsetRange()
{
    ui->valOffset->setScale(-OFFSET_TURNS * OFFSET_TICKS_PER_TURN, OFFSET_TURNS * OFFSET_TICKS_PER_TURN);
    ui->valOffset->setTotalSteps(OFFSET_TURNS * OFFSET_TICKS_PER_TURN + OFFSET_TURNS * OFFSET_TICKS_PER_TURN);
    ui->valOffset->setNumTurns(OFFSET_TURNS + OFFSET_TURNS);
}
