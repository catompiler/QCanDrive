#ifndef TRENDPLOTEDITDLG_H
#define TRENDPLOTEDITDLG_H

#include <QDialog>

namespace Ui {
class TrendPlotEditDlg;
}

class TrendPlotEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TrendPlotEditDlg(QWidget *parent = nullptr);
    ~TrendPlotEditDlg();

private slots:
    void on_slTransp_valueChanged(int value);
    void on_tbColorSel_clicked(bool checked = false);

private:
    Ui::TrendPlotEditDlg *ui;
};

#endif // TRENDPLOTEDITDLG_H
