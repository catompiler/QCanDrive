#ifndef OSCOPEAXISWGT_H
#define OSCOPEAXISWGT_H

#include <QWidget>
#include <QPair>

namespace Ui {
class OScopeAxisWgt;
}

class OScopeAxisWgt : public QWidget
{
    Q_OBJECT

public:

    static constexpr int SCALE_VALUES[] = {1, 2, 5};
    static constexpr int SCALE_LEN = sizeof(SCALE_VALUES)/sizeof(SCALE_VALUES[0]);
    static constexpr qreal OFFSET_K = 1.0 / 25.0;

    static constexpr int SCALE_STEPS = 20;
    static constexpr int SCALE_TURNS = 4;
    static constexpr int OFFSET_STEPS = 100;
    static constexpr int OFFSET_TURNS = 10;

    explicit OScopeAxisWgt(QWidget *parent = nullptr);
    ~OScopeAxisWgt();

    QString scaleTitle() const;
    void setScaleTitle(const QString& newScaleTitle);

    QString offsetTitle() const;
    void setOffsetTitle(const QString& newOffsetTitle);

    QString unit() const;
    void setUnit(const QString& newUnit);

    qreal scaleMin() const;
    void setScaleMin(qreal newScaleMin);

    qreal scaleMax() const;
    void setScaleMax(qreal newScaleMax);

    qreal scale() const;
    void setScale(qreal newScale);

    qreal offsetMin() const;
    void setOffsetMin(qreal newOffsetMin);

    qreal offsetMax() const;
    void setOffsetMax(qreal newOffsetMax);

    qreal offset() const;
    void setOffset(qreal newOffset);

signals:
    void scaleChanged(qreal value);
    void offsetChanged(qreal value);

private slots:
    void scale_valueChanged(double value);
    void offset_valueChanged(double value);

private:
    Ui::OScopeAxisWgt *ui;

    QString m_scaleTitle;
    QString m_offsetTitle;

    QString m_unit;

    qreal m_scaleMin;
    qreal m_scaleMax;
    qreal m_scale;
    int m_scaleExp;

    qreal m_offsetMin;
    qreal m_offsetMax;
    qreal m_offset;
    int m_offsetExp;

    QPair<qreal, int> valueScaleBase(qreal val, qreal scaleBase) const;

    qreal valueFromScaleIndex(int idx) const;
    qreal valueFromOffsetIndex(int idx) const;

    int scaleIndexFromValue(qreal scaleVal) const;
    int offsetIndexFromValue(qreal offsetVal) const;

    void updateScaleDispVal();
    void updateOffsetDispVal();
};

#endif // OSCOPEAXISWGT_H
