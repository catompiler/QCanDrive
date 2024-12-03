#ifndef SDOVALUEDIALEDITDLG_H
#define SDOVALUEDIALEDITDLG_H

#include <QDialog>
#include <QColor>
#include "cotypes.h"
#include "covaluetypes.h"


namespace Ui {
class SDOValueDialEditDlg;
}

class SDOValueDialEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SDOValueDialEditDlg(QWidget *parent = nullptr);
    ~SDOValueDialEditDlg();

    QString name() const;
    void setName(const QString& newName);

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

    qreal rangeMin() const;
    void setRangeMin(qreal newRangeMin);

    qreal rangeMax() const;
    void setRangeMax(qreal newRangeMax);

    qreal penWidth() const;
    void setPenWidth(qreal newPenWidth);

    uint precision() const;
    void setPrecision(uint newPrecision);

private slots:
    void on_tbOutsideBackColorSel_clicked(bool checked = false);
    void on_tbInsideBackColorSel_clicked(bool checked = false);
    void on_tbInsideScaleBackColorSel_clicked(bool checked = false);
    void on_tbTextScaleColorSel_clicked(bool checked = false);
    void on_tbNeedleColorSel_clicked(bool checked = false);

private:
    Ui::SDOValueDialEditDlg *ui;


    void peekColor(QWidget* colHolder);
    void populateTypes();
};

#endif // SDOVALUEDIALEDITDLG_H
