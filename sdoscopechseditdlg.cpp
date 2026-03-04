#include "sdoscopechseditdlg.h"
#include "ui_sdoscopechseditdlg.h"

SDOScopeChsEditDlg::SDOScopeChsEditDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SDOScopeChsEditDlg)
{
    ui->setupUi(this);
}

SDOScopeChsEditDlg::~SDOScopeChsEditDlg()
{
    delete ui;
}
