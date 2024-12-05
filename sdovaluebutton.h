#ifndef SDOVALUEBUTTON_H
#define SDOVALUEBUTTON_H

#include <QAbstractButton>
#include "cotypes.h"
#include "covaluetypes.h"
#include "covaluesholder.h"


class SDOValue;
class QImage;


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

    QImage* m_imgNormal;
    QImage* m_imgChecked;
    QImage* m_imgNormalClicked;
    QImage* m_imgCheckedClicked;
    QImage* m_imgMouse;
    QImage* m_imgFocus;

    int m_borderWidth;

    bool m_mouseFlag;
    bool m_clickFlag;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    QColor lerpColor(const QColor& ca, const QColor& cb, int t = 256);

    void createImages();
    void deleteImages();
    void updateImages();

    void drawButton(QPainter* p);

    void drawText(QPainter* p);

    void drawChecked(QPainter* p);

    void drawBorder(QPainter* p);
    void drawTickRect(QPainter* p, const QRect& r, int w);

    void drawNormal(QPainter* p);

    void drawBorderGrad(QPainter* p, int borderWidth, const QColor& btnCol, const QColor& brdrCol);
    void drawHighlight(QPainter* p);

    void drawMouse(QPainter* p);
    void drawNormalClicked(QPainter* p);
    void drawCheckedClicked(QPainter* p);
};

#endif // SDOVALUEBUTTON_H
