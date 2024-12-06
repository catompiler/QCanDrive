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

    QColor buttonColor() const;
    void setButtonColor(const QColor& newButtonColor);

    QColor borderColor() const;
    void setBorderColor(const QColor& newBorderColor);

    QColor indicatorColor() const;
    void setIndicatorColor(const QColor& newIndicatorColor);

    QColor activateColor() const;
    void setActivateColor(const QColor& newActivateColor);

    QColor highlightColor() const;
    void setHighlightColor(const QColor& newHlColor);

    QColor textColor() const;
    void setTextColor(const QColor& newTextColor);

    int borderWidth() const;
    void setBorderWidth(int newBorderWidth);

    bool indicatorEnabled() const;
    void setIndicatorEnabled(bool newEnabled);

    bool indicatorActive() const;
    void setIndicatorActive(bool newActive);

    int fontPointSize() const;
    void setFontPointSize(int newSize);

    bool fontCapitalization() const;
    void setFontCapitalization(bool newCap);

    bool fontBold() const;
    void setFontBold(bool newBold);

    uint32_t activatedValueMask() const;
    void setActivatedValueMask(uint32_t newActivatedValueMask);

    bool setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType);
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
    bool m_updateMask;

    QImage* m_imgBuffer;
    QImage* m_imgBorder;
    QImage* m_imgNormal;
    QImage* m_imgChecked;
    QImage* m_imgClicked;
    QImage* m_imgMouse;
    QImage* m_imgFocus;

    int m_borderWidth;
    uint32_t m_activatedValueMask;

    bool m_mouseFlag;
    bool m_clickFlag;

    void setupRdSdoValue();

    void onClick();

    bool event(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
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
    void drawClicked(QPainter* p);
};

#endif // SDOVALUEBUTTON_H
