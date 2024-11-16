#include "canopenwin.h"
#include "ui_canopenwin.h"

CanOpenWin::CanOpenWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CanOpenWin)
{
    ui->setupUi(this);
}

CanOpenWin::~CanOpenWin()
{
    delete ui;
}

