#include "flagseditdlg.h"
#include "ui_flagseditdlg.h"
#include "flagseditmodel.h"



FlagsEditDlg::FlagsEditDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FlagsEditDlg)
{
    ui->setupUi(this);
    m_flagsEditModel = new FlagsEditModel();
    ui->tvFlags->setModel(m_flagsEditModel);
}

FlagsEditDlg::~FlagsEditDlg()
{
    delete m_flagsEditModel;
    delete ui;
}

QStringList FlagsEditDlg::flagsNames() const
{
    return m_flagsEditModel->flagsNames();
}

void FlagsEditDlg::setFlagsNames(const QStringList& newFlagsNames)
{
    m_flagsEditModel->setFlagsNames(newFlagsNames);
}

uint FlagsEditDlg::flagsValues() const
{
    return m_flagsEditModel->flagsValues();
}

void FlagsEditDlg::setFlagsValues(uint newFlagsValues)
{
    m_flagsEditModel->setFlagsValues(newFlagsValues);
}
