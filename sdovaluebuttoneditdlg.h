#ifndef SDOVALUEBUTTONEDITDLG_H
#define SDOVALUEBUTTONEDITDLG_H

#include <QDialog>
#include <QColor>
#include "cotypes.h"
#include "covaluetypes.h"
#include "sdovaluebutton.h"


namespace Ui {
class SDOValueButtonEditDlg;
}

class SDOValueButtonEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SDOValueButtonEditDlg(QWidget *parent = nullptr);
    ~SDOValueButtonEditDlg();

    QString text() const;
    void setText(const QString& newName);

    CO::NodeId nodeId() const;
    void setNodeId(CO::NodeId newNodeId);

    CO::Index index() const;
    void setIndex(CO::Index newIndex);

    CO::SubIndex subIndex() const;
    void setSubIndex(CO::SubIndex newSubIndex);

    COValue::Type type() const;
    void setType(COValue::Type newType);

    int posRow() const;
    void setPosRow(int newPosRow);

    int posColumn() const;
    void setPosColumn(int newPosColumn);

    int sizeRows() const;
    void setSizeRows(int newSizeRows);

    int sizeColumns() const;
    void setSizeColumns(int newSizeColumns);

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

    int fontPointSize() const;
    void setFontPointSize(int newSize);

    bool fontCapitalization() const;
    void setFontCapitalization(bool newCap);

    bool fontBold() const;
    void setFontBold(bool newBold);

    SDOValueButton::CompareType indicatorCompare() const;
    void setIndicatorCompare(SDOValueButton::CompareType newIndicatorCompare);

    uint32_t indicatorValue() const;
    void setIndicatorValue(uint32_t newIndicatorValue);

    uint32_t activateValue() const;
    void setActivateValue(uint32_t newActivateValue);

private slots:
    void on_tbButtonColorSel_clicked(bool checked = false);
    void on_tbBorderColorSel_clicked(bool checked = false);
    void on_tbIndicatorColorSel_clicked(bool checked = false);
    void on_tbActivateColorSel_clicked(bool checked = false);
    void on_tbHighlightColorSel_clicked(bool checked = false);
    void on_tbTextColorSel_clicked(bool checked = false);

private:
    Ui::SDOValueButtonEditDlg *ui;

    void peekColor(QWidget* colHolder);
    void populateTypes();
    void populateCompares();
};

#endif // SDOVALUEBUTTONEDITDLG_H
