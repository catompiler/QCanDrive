#include "reglisteditorwin.h"
#include "ui_reglisteditorwin.h"
#include "reglistmodel.h"


RegListEditorWin::RegListEditorWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RegListEditorWin)
{
    ui->setupUi(this);

    regsListModel = new RegListModel();
    ui->tvRegList->setModel(regsListModel);
}

RegListEditorWin::~RegListEditorWin()
{
    delete ui;
    delete regsListModel;
}

