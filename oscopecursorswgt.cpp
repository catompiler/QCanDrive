#include "oscopecursorswgt.h"
#include "ui_oscopecursorswgt.h"

OScopeCursorsWgt::OScopeCursorsWgt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OScopeCursorsWgt)
{
    ui->setupUi(this);

    connect(ui->cbFloating, &QCheckBox::checkStateChanged, this, &OScopeCursorsWgt::floatingEnabled_stateChanged);
}

OScopeCursorsWgt::~OScopeCursorsWgt()
{
    delete ui;
}

bool OScopeCursorsWgt::floatingEnabled() const
{
    return ui->cbFloating->isChecked();
}

void OScopeCursorsWgt::setFloatingEnabled(bool newEnabled)
{
    ui->cbFloating->setChecked(newEnabled);
}

void OScopeCursorsWgt::floatingEnabled_stateChanged(Qt::CheckState newState)
{
    switch(newState){
    default:
        break;
    case Qt::Unchecked:
        emit floatingEnabledChanged(false);
        break;
    case Qt::Checked:
        emit floatingEnabledChanged(true);
        break;
    }
}
