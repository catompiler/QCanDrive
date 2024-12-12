#ifndef SDOVALUEINDICATOR_H
#define SDOVALUEINDICATOR_H

#include <QWidget>
#include "cotypes.h"
#include "covaluetypes.h"
#include "covaluesholder.h"


class SDOValue;
class QImage;


class SDOValueIndicator : public QWidget
{
    Q_OBJECT
public:
    SDOValueIndicator(CoValuesHolder* newValsHolder = nullptr, QWidget* parent = nullptr);
    ~SDOValueIndicator();

    CoValuesHolder* valuesHolder() const;
    void setValuesHolder(CoValuesHolder* newValuesHolder);

    QString text() const;
    void setText(const QString& newText);

    QColor backColor() const;
    void setBackColor(const QColor& newBackColor);

    QColor shadowColor() const;
    void setShadowColor(const QColor& newShadowColor);

    QColor indicatorColor() const;
    void setIndicatorColor(const QColor& newIndicatorColor);

    QColor glareColor() const;
    void setGlareColor(const QColor& newGlareColor);

    QColor textColor() const;
    void setTextColor(const QColor& newTextColor);

    int borderWidth() const;
    void setBorderWidth(int newBorderWidth);

    int fontPointSize() const;
    void setFontPointSize(int newSize);

    bool fontCapitalization() const;
    void setFontCapitalization(bool newCap);

    bool fontBold() const;
    void setFontBold(bool newBold);

    enum CompareType : uint {
        NOT_EQUAL = 0,
        EQUAL = 1,
        LESS = 2,
        LESS_EQUAL = 3,
        GREATER = 4,
        GREATER_EQUAL = 5,
        MASK = 6,
        MASK_ZERO = 7
    };

    CompareType indicatorCompare() const;
    void setIndicatorCompare(CompareType newIndicatorCompare);

    uint32_t indicatorValue() const;
    void setIndicatorValue(uint32_t newIndicatorValue);

    bool setSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type newType);
    CoValuesHolder::HoldedSDOValuePtr getSDOValue();
    CoValuesHolder::HoldedSDOValuePtr getSDOValue() const;
    COValue::Type SDOValueType() const;
    void resetSDOValue();

    bool indicatorActive() const;
    void setIndicatorActive(bool newActive);

    QSize sizeHint() const override;

    void applyAppearance();

private slots:
    void sdovalueReaded();

protected:
    CoValuesHolder* m_valsHolder;
    CoValuesHolder::HoldedSDOValuePtr m_sdoValue;
    COValue::Type m_sdoValueType;

    QImage* m_imgBuffer;
    QImage* m_imgNormal;
    QImage* m_imgActive;

    QString m_text;
    int m_borderWidth;

    CompareType m_indicatorCompare;
    uint32_t m_indicatorValue;
    bool m_indicatorActive;

    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    QColor lerpColor(const QColor& ca, const QColor& cb, int t = 256);

    void createImages();
    void deleteImages();
    void updateImages();

    void drawIndicator(QPainter* p);

    void drawText(QPainter* p);

    void drawNormal(QPainter* p);

    void drawActive(QPainter* p);

    void drawGrad(QPainter* p, int borderWidth, const QColor& glareCol, const QColor& shadowCol, qreal glarePos = 0.25);
};

#endif // SDOVALUEINDICATOR_H
