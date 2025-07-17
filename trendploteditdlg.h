#ifndef TRENDPLOTEDITDLG_H
#define TRENDPLOTEDITDLG_H

#include <QDialog>

namespace Ui {
class TrendPlotEditDlg;
}

class SignalCurveEditDlg;
class SignalCurvePropModel;
class SignalCurveProp;


class TrendPlotEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TrendPlotEditDlg(QWidget *parent = nullptr);
    ~TrendPlotEditDlg();

    SignalCurveEditDlg* signalCurveEditDialog() const;
    void setSignalCurveEditDialog(SignalCurveEditDlg* newSignalCurveEditDialog);

    int samplesCount() const;
    void setSamplesCount(int newCount);

    QString plotName() const;
    void setPlotName(const QString& newName);

    QColor backColor() const;
    void setBackColor(const QColor& newBackColor);

    QColor textColor() const;
    void setTextColor(const QColor& newTextColor);

    int defaultAlpha() const;
    void setDefaultAlpha(int defaultAlpha);

    int posRow() const;
    void setPosRow(int newPosRow);

    int posColumn() const;
    void setPosColumn(int newPosColumn);

    int sizeRows() const;
    void setSizeRows(int newSizeRows);

    int sizeColumns() const;
    void setSizeColumns(int newSizeColumns);

    int signalsCount() const;

    const SignalCurveProp& signalCurveProp(int n) const;
    void setSignalCurveProp(int n, const SignalCurveProp& newSignalCurveProp);

    void setSignalsCount(int newSignalsCount);

    bool legendEnabled() const;
    void setLegendEnabled(bool newLegendEnabled);

private slots:
    void on_slTransp_valueChanged(int value);
    void on_tbColorSel_clicked(bool checked = false);
    void on_tbTextColorSel_clicked(bool checked = false);
    void on_pbAdd_clicked(bool checked = false);
    void on_pbEdit_clicked(bool checked = false);
    void on_pbUp_clicked(bool checked = false);
    void on_pbDown_clicked(bool checked = false);
    void on_pbDel_clicked(bool checked = false);

private:
    Ui::TrendPlotEditDlg *ui;
    SignalCurvePropModel* m_signalCurvePropMdl;

    SignalCurveEditDlg* m_signalCurveEditDlg;
};

#endif // TRENDPLOTEDITDLG_H
