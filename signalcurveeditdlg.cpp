#include "signalcurveeditdlg.h"
#include "ui_signalcurveeditdlg.h"

SignalCurveEditDlg::SignalCurveEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignalCurveEditDlg)
{
    ui->setupUi(this);
}

SignalCurveEditDlg::~SignalCurveEditDlg()
{
    delete ui;
}
