#ifndef SDOVALUESLIDER_H
#define SDOVALUESLIDER_H

#include <QwtSlider>
#include "cotypes.h"
#include "covaluetypes.h"
#include "covaluesholder.h"


class SDOValue;


class SDOValueSlider : public QwtSlider
{
    Q_OBJECT
public:
    SDOValueSlider(CoValuesHolder* newValsHolder = nullptr, QWidget* parent = nullptr);
    ~SDOValueSlider();

    QString name() const;
    void setName(const QString& newName);

    CoValuesHolder* valuesHolder() const;
    void setValuesHolder(CoValuesHolder* newValuesHolder);

    QColor troughColor() const;
    void setTroughColor(const QColor& newTroughColor);

    QColor grooveColor() const;
    void setGrooveColor(const QColor& newGrooveColor);

    QColor handleColor() const;
    void setHandleColor(const QColor& newHandleColor);

    QColor scaleColor() const;
    void setScaleColor(const QColor& newScaleColor);

    QColor textColor() const;
    void setTextColor(const QColor& newTextColor);

    qreal penWidth() const;
    void setPenWidth(qreal newPenWidth);

    uint steps() const;
    void setSteps(uint newSteps);

    bool hasTrough() const;
    void setHasTrough(bool newHasTrough);

    bool hasGroove() const;
    void setHasGroove(bool newHasGroove);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation newOrientation);

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
    CoValuesHolder::HoldedSDOValuePtr m_rdSdoValue;
    SDOValue* m_wrSdoValue;
    COValue::Type m_sdoValueType;
    QString m_name;
    bool m_updateMask;

    void updateContentMargins();

    void paintEvent(QPaintEvent* event) override;
    void sliderChange() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif // SDOVALUESLIDER_H
