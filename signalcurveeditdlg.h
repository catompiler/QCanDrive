#ifndef SIGNALCURVEEDITDLG_H
#define SIGNALCURVEEDITDLG_H

#include <QDialog>
#include <QString>
#include <QColor>
#include "cotypes.h"
#include "covaluetypes.h"



namespace Ui {
class SignalCurveEditDlg;
}


class SignalCurveEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SignalCurveEditDlg(QWidget *parent = nullptr);
    ~SignalCurveEditDlg();

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

    QColor penColor() const;
    void setPenColor(const QColor& newPenColor);

    Qt::PenStyle penStyle() const;
    void setPenStyle(Qt::PenStyle newPenStyle);

    qreal penWidth() const;
    void setPenWidth(qreal newPenWidth);

    QColor brushColor() const;
    void setBrushColor(const QColor& newBrushColor);

    Qt::BrushStyle brushStyle() const;
    void setBrushStyle(Qt::BrushStyle newBrushStyle);

private slots:
    void on_tbPenColorSel_clicked(bool checked = false);
    void on_tbBrushColorSel_clicked(bool checked = false);

private:
    Ui::SignalCurveEditDlg *ui;

    void populateTypes();
    void populatePenStyles();
    void populateBrushStyles();
};

#endif // SIGNALCURVEEDITDLG_H
