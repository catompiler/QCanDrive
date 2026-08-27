#ifndef OSCOPEAXISWGT_H
#define OSCOPEAXISWGT_H

#include <QWidget>
#include <QPair>
#include <QStringList>

class UnitFormatter;

namespace Ui {
class OScopeAxisWgt;
}

class OScopeAxisWgt : public QWidget
{
    Q_OBJECT

public:

    static constexpr int SCALE_VALUES[] = {1, 2, 5};
    static constexpr int SCALE_LEN = sizeof(SCALE_VALUES)/sizeof(SCALE_VALUES[0]);
    static constexpr int OFFSET_TICKS_PER_TURN = 25;
    static constexpr qreal OFFSET_K = 1.0 / OFFSET_TICKS_PER_TURN;
    static constexpr int OFFSET_TURNS = 100;

    enum AdjustType {
        ADJUST_TO_LOWEST = -1,
        ADJUST_TO_NEAREST = 0,
        ADJUST_TO_HIGHEST = 1
    };

    explicit OScopeAxisWgt(QWidget *parent = nullptr);
    ~OScopeAxisWgt();

    QString scaleTitle() const;
    void setScaleTitle(const QString& newScaleTitle);

    QString offsetTitle() const;
    void setOffsetTitle(const QString& newOffsetTitle);

    UnitFormatter* unitFormatter() const;
    //! Устанавливает форматирователь значения.
    //! Берёт во владение, удаляя предыдущий.
    void setUnitFormatter(UnitFormatter* newFormatter);

    qreal scaleMin() const;
    void setScaleMin(qreal newScaleMin);

    qreal scaleMax() const;
    void setScaleMax(qreal newScaleMax);

    qreal scale() const;
    void setScale(qreal newScale);

    int scaleTurns() const;
    void setScaleTurns(int newScaleTurns);

    qreal offset() const;
    void setOffset(qreal newOffset);

    AdjustType scaleAdjustType() const;
    void setScaleAdjustType(AdjustType newType);

signals:
    void scaleChanged(qreal value);
    void offsetChanged(qreal value);

private slots:
    void scale_valueChanged(double value);
    void offset_valueChanged(double value);
    void scale_doubleClicked(int buttons, int modifiers);
    void offset_doubleClicked(int buttons, int modifiers);

private:
    Ui::OScopeAxisWgt *ui;

    QString m_scaleTitle;
    QString m_offsetTitle;

    UnitFormatter* m_unitFmt;

    AdjustType m_scaleAdjustType;

    QPair<qreal, int> valueScaleBase(qreal val, qreal scaleBase) const;

    qreal valueFromScaleIndex(int idx) const;
    qreal valueFromOffsetIndex(int idx) const;

    int scaleIndexFromValue(qreal scaleVal) const;
    int offsetIndexFromValue(qreal offsetVal) const;

    QString getDispVal(qreal dispVal) const;

    void updateScaleDispVal();
    void updateOffsetDispVal();

    void updateOffsetRange();
};

#endif // OSCOPEAXISWGT_H
