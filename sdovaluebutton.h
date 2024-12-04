#ifndef SDOVALUEBUTTON_H
#define SDOVALUEBUTTON_H

#include <QAbstractButton>
#include "cotypes.h"
#include "covaluetypes.h"
#include "covaluesholder.h"


class SDOValue;


class SDOValueButton : public QAbstractButton
{
    Q_OBJECT
public:
    SDOValueButton(CoValuesHolder* newValsHolder = nullptr, QWidget* parent = nullptr);
    ~SDOValueButton();

    CoValuesHolder* valuesHolder() const;
    void setValuesHolder(CoValuesHolder* newValuesHolder);

    QColor buttonBackColor() const;
    void setButtonBackColor(const QColor& newButtonBackColor);

    QColor buttonColor() const;
    void setButtonColor(const QColor& newButtonColor);

    QColor buttonAlarmColor() const;
    void setButtonAlarmColor(const QColor& newButtonAlarmColor);

    QColor scaleColor() const;
    void setScaleColor(const QColor& newScaleColor);

    QColor textColor() const;
    void setTextColor(const QColor& newTextColor);

    int buttonWidth() const;
    void setButtonWidth(int newButtonWidth);

    bool setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType);
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

    int m_borderWidth;

    void paintEvent(QPaintEvent* event) override;

    void drawButton(QPainter* p);
    void drawButtonIdle(QPainter* p);
    void drawBorder(QPainter* p, int borderWidth, const QColor& btnCol, const QColor& brdrCol);
    void drawCenter(QPainter* p);
    void drawHighlight(QPainter* p);
};

#endif // SDOVALUEBUTTON_H
