#include "oscopechannelwgt.h"
#include "ui_oscopechannelwgt.h"

OScopeChannelWgt::OScopeChannelWgt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OScopeChannelWgt)
{
    ui->setupUi(this);
}

OScopeChannelWgt::~OScopeChannelWgt()
{
    delete ui;
}
