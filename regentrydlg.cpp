#include "regentrydlg.h"
#include "ui_regentrydlg.h"
#include "regtypes.h"



RegEntryDlg::RegEntryDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegEntryDlg)
{
    ui->setupUi(this);
    populateTypesList();
}

RegEntryDlg::~RegEntryDlg()
{
    delete ui;
}

reg_index_t RegEntryDlg::index() const
{
    return ui->sbIndex->value();
}

void RegEntryDlg::setIndex(reg_index_t newIndex)
{
    ui->sbIndex->setValue(static_cast<int>(newIndex));
}

ObjectType RegEntryDlg::objectType() const
{
    return static_cast<ObjectType>(ui->cbType->currentData().toUInt());
}

void RegEntryDlg::setObjectType(ObjectType newObjectType)
{
    for(int i = 0; i < ui->cbType->count(); i ++){
        if(ui->cbType->itemData(i, Qt::UserRole).toInt() == static_cast<int>(newObjectType)){
            ui->cbType->setCurrentIndex(i);
            break;
        }
    }
}

QString RegEntryDlg::name() const
{
    return ui->leName->text();
}

void RegEntryDlg::setName(const QString &newName)
{
    ui->leName->setText(newName);
}

QString RegEntryDlg::description() const
{
    return ui->teDescr->toPlainText();
}

void RegEntryDlg::setDescription(const QString &newDescription)
{
    ui->teDescr->setPlainText(newDescription);
}

void RegEntryDlg::populateTypesList()
{
    auto obj_types = RegTypes::objectTypes();

    for(auto obj_type: obj_types){
        ui->cbType->addItem(RegTypes::typeStr(obj_type), static_cast<int>(obj_type));
    }
}
