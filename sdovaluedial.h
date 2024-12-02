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

    bool setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType, qreal newMin = 0.0, qreal newMax = 1.0);
    void resetSDOValue();

private slots:
    void sdovalueReaded();

protected:
    CoValuesHolder* m_valsHolder;
    CoValuesHolder::HoldedSDOValuePtr m_sdoValue;
    COValue::Type m_sdoValueType;

    qreal m_penWidth;
};

#endif // SDOVALUEDIAL_H
