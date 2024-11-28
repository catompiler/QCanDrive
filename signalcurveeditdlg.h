#ifndef SIGNALCURVEEDITDLG_H
#define SIGNALCURVEEDITDLG_H

#include <QDialog>

namespace Ui {
class SignalCurveEditDlg;
}

class SignalCurveEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SignalCurveEditDlg(QWidget *parent = nullptr);
    ~SignalCurveEditDlg();

private:
    Ui::SignalCurveEditDlg *ui;
};

#endif // SIGNALCURVEEDITDLG_H
