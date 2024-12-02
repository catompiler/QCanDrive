#ifndef SDOVALUEDIAL_H
#define SDOVALUEDIAL_H


#include <QwtDial>
#include "cotypes.h"
#include "covaluetypes.h"
#include "covaluesholder.h"


class SDOValue;



class SDOValueDial : public QwtDial
{
    Q_OBJECT
public:
    SDOValueDial(CoValuesHolder* newValsHolder = nullptr, QWidget* parent = nullptr);
    ~SDOValueDial();

    QString name() const;
    void setName(const QString& newName);

    CoValuesHolder* valuesHolder() const;
    void setValuesHolder(CoValuesHolder* newValuesHolder);

    QColor outsideBackColor() const;
    void setOutsideBackColor(const QColor& newBackOuterColor);

    QColor insideBackColor() const;
    void setInsideBackColor(const QColor& newBackInnerColor);

    QColor insideScaleBackColor() const;
    void setInsideScaleBackColor(const QColor& newBackScaleColor);

    QColor textScaleColor() const;
    void setTextScaleColor(const QColor& newTextScaleColor);

    QColor needleColor() const;
    void setNeedleColor(const QColor& newNeedleColor);

    qreal penWidth() const;
    void setPenWidth(qreal newPenWidth);

    qreal rangeMin() const;
    void setRangeMin(qreal newRangeMin);

    qreal rangeMax() const;
    void setRangeMax(qreal newRangeMax);

    bool setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType, qreal newMin = 0.0, qreal newMax = 1.0);
    CoValuesHolder::HoldedSDOValuePtr SDOValue();
    CoValuesHolder::HoldedSDOValuePtr SDOValue() const;
    COValue::Type SDOValueType() const;
    void resetSDOValue();

private slots:
    void sdovalueReaded();

protected:
    CoValuesHolder* m_valsHolder;
    CoValuesHolder::HoldedSDOValuePtr m_sdoValue;
    COValue::Type m_sdoValueType;

    QString m_name;

    void drawScaleContents(QPainter* painter, const QPointF& center, double radius) const override;
};

#endif // SDOVALUEDIAL_H
