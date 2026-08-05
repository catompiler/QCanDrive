#include "oscopeaxiswgt.h"
#include "ui_oscopeaxiswgt.h"

OScopeAxisWgt::OScopeAxisWgt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OScopeAxisWgt)
{
    ui->setupUi(this);
}

OScopeAxisWgt::~OScopeAxisWgt()
{
    delete ui;
}
