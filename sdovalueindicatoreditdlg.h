#ifndef SDOVALUEINDICATOREDITDLG_H
#define SDOVALUEINDICATOREDITDLG_H

#include <QDialog>
#include <QColor>
#include "cotypes.h"
#include "covaluetypes.h"
#include "sdovalueindicator.h"


namespace Ui {
class SDOValueIndicatorEditDlg;
}

class SDOValueIndicatorEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SDOValueIndicatorEditDlg(QWidget *parent = nullptr);
    ~SDOValueIndicatorEditDlg();

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

    SDOValueIndicator::CompareType indicatorCompare() const;
    void setIndicatorCompare(SDOValueIndicator::CompareType newIndicatorCompare);

    uint32_t indicatorValue() const;
    void setIndicatorValue(uint32_t newIndicatorValue);

private slots:
    void on_tbBackColorSel_clicked(bool checked = false);
    void on_tbShadowColorSel_clicked(bool checked = false);
    void on_tbIndicatorColorSel_clicked(bool checked = false);
    void on_tbGlareColorSel_clicked(bool checked = false);
    void on_tbTextColorSel_clicked(bool checked = false);

private:
    Ui::SDOValueIndicatorEditDlg *ui;

    void peekColor(QWidget* colHolder);
    void populateTypes();
    void populateCompares();
};

#endif // SDOVALUEINDICATOREDITDLG_H
