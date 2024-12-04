#ifndef SDOVALUEBAREDITDLG_H
#define SDOVALUEBAREDITDLG_H

#include <QDialog>
#include <QColor>
#include <QwtThermo>
#include "cotypes.h"
#include "covaluetypes.h"


namespace Ui {
class SDOValueBarEditDlg;
}

class SDOValueBarEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SDOValueBarEditDlg(QWidget *parent = nullptr);
    ~SDOValueBarEditDlg();

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

    QColor barBackColor() const;
    void setBarBackColor(const QColor& newBarBackColor);

    QColor barColor() const;
    void setBarColor(const QColor& newBarColor);

    QColor barAlarmColor() const;
    void setBarAlarmColor(const QColor& newBarAlarmColor);

    QColor scaleColor() const;
    void setScaleColor(const QColor& newScaleColor);

    QColor textColor() const;
    void setTextColor(const QColor& newTextColor);

    qreal rangeMin() const;
    void setRangeMin(qreal newRangeMin);

    qreal rangeMax() const;
    void setRangeMax(qreal newRangeMax);

    int barWidth() const;
    void setBarWidth(int newBarWidth);

    int borderWidth() const;
    void setBorderWidth(int newBorderWidth);

    qreal penWidth() const;
    void setPenWidth(qreal newPenWidth);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation newOrient);

    QwtThermo::ScalePosition scalePosition() const;
    void setScalePosition(QwtThermo::ScalePosition newPos);

    bool alarmEnabled() const;
    void setAlarmEnabled(bool newEnabled);

    qreal alarmLevel() const;
    void setAlarmLevel(qreal newLevel);

private slots:
    void on_tbBarBackColorSel_clicked(bool checked = false);
    void on_tbBarColorSel_clicked(bool checked = false);
    void on_tbBarAlarmColorSel_clicked(bool checked = false);
    void on_tbScaleColorSel_clicked(bool checked = false);
    void on_tbTextColorSel_clicked(bool checked = false);
    void on_sbRangeMin_valueChanged(double d);
    void on_sbRangeMax_valueChanged(double d);

private:
    Ui::SDOValueBarEditDlg *ui;

    void peekColor(QWidget* colHolder);
    void populateTypes();
    void populateOrientations();
    void populateScalePositions();
};

#endif // SDOVALUEBAREDITDLG_H
