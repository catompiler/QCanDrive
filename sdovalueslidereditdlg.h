#ifndef SDOVALUESLIDEREDITDLG_H
#define SDOVALUESLIDEREDITDLG_H

#include <QDialog>
#include <QColor>
#include "cotypes.h"
#include "covaluetypes.h"


namespace Ui {
class SDOValueSliderEditDlg;
}

class SDOValueSliderEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SDOValueSliderEditDlg(QWidget *parent = nullptr);
    ~SDOValueSliderEditDlg();

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

    qreal rangeMin() const;
    void setRangeMin(qreal newRangeMin);

    qreal rangeMax() const;
    void setRangeMax(qreal newRangeMax);

    qreal penWidth() const;
    void setPenWidth(qreal newPenWidth);

    uint steps() const;
    void setSteps(uint newSteps);

    bool hasTrough() const;
    void setHasTrough(bool newHasTrough);

    bool hasGroove() const;
    void setHasGroove(bool newHasGroove);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation newAlign);

private slots:
    void on_tbTroughColorSel_clicked(bool checked = false);
    void on_tbGrooveColorSel_clicked(bool checked = false);
    void on_tbHandleColorSel_clicked(bool checked = false);
    void on_tbScaleColorSel_clicked(bool checked = false);
    void on_tbTextColorSel_clicked(bool checked = false);

private:
    Ui::SDOValueSliderEditDlg *ui;

    void peekColor(QWidget* colHolder);
    void populateTypes();
    void populateAlignments();
};

#endif // SDOVALUESLIDEREDITDLG_H
