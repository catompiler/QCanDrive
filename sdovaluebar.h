#ifndef SDOVALUEBAR_H
#define SDOVALUEBAR_H

#include <QwtThermo>
#include "cotypes.h"
#include "covaluetypes.h"
#include "covaluesholder.h"


class SDOValue;


class SDOValueBar : public QwtThermo
{
    Q_OBJECT
public:
    SDOValueBar(CoValuesHolder* newValsHolder = nullptr, QWidget* parent = nullptr);
    ~SDOValueBar();

    QString name() const;
    void setName(const QString& newName);

    CoValuesHolder* valuesHolder() const;
    void setValuesHolder(CoValuesHolder* newValuesHolder);

    QColor barBackColor() const;
    void setBarBackColor(const QColor& newBarBackColor);

    QColor barColor() const;
    void setBarColor(const QColor& newBarColor);

    QColor barAlarmColor() const;
    void setBarAlarmColor(const QColor& newBarAlarmColor);

    QColor scaleColor() const;
    void setScaleColor(const QColor& newScaleColor);

    QColor textColor() const;
    void setTextColor(const QColor& newTextColor);

    int barWidth() const;
    void setBarWidth(int newBarWidth);

    using QwtThermo::borderWidth;
    using QwtThermo::setBorderWidth;

    qreal penWidth() const;
    void setPenWidth(qreal newPenWidth);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation newOrientation);

    QwtThermo::ScalePosition scalePosition() const;
    void setScalePosition(QwtThermo::ScalePosition newPos);

    using QwtThermo::setAlarmLevel;
    using QwtThermo::alarmLevel;

    bool alarmEnabled() const;
    void setAlarmEnabled(bool newEnabled);

    qreal rangeMin() const;
    void setRangeMin(qreal newRangeMin);

    qreal rangeMax() const;
    void setRangeMax(qreal newRangeMax);

    bool setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType, qreal newMin = 0.0, qreal newMax = 1.0);
    CoValuesHolder::HoldedSDOValuePtr getSDOValue();
    CoValuesHolder::HoldedSDOValuePtr getSDOValue() const;
    COValue::Type SDOValueType() const;
    void resetSDOValue();

private slots:
    void sdovalueReaded();

protected:
    CoValuesHolder* m_valsHolder;
    CoValuesHolder::HoldedSDOValuePtr m_sdoValue;
    COValue::Type m_sdoValueType;
    QString m_name;

    void updateContentMargins();

    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;
};

#endif // SDOVALUEBAR_H
