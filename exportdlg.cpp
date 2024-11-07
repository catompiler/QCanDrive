#include "exportdlg.h"
#include "ui_exportdlg.h"
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>


ExportDlg::ExportDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDlg)
{
    ui->setupUi(this);

    ui->lePath->setText(QDir::currentPath());
}

ExportDlg::~ExportDlg()
{
    delete ui;
}

QString ExportDlg::path() const
{
    return ui->lePath->text();
}

void ExportDlg::setPath(const QString& newPath)
{
    ui->lePath->setText(newPath);
}

QString ExportDlg::regIdsFileName() const
{
    return ui->leRegIds->text();
}

void ExportDlg::setRegIdsFileName(const QString& newRegIdsFileName)
{
    ui->leRegIds->setText(newRegIdsFileName);
}

QString ExportDlg::regListFileName() const
{
    return ui->leRegList->text();
}

void ExportDlg::setRegListFileName(const QString& newRegListFileName)
{
    ui->leRegList->setText(newRegListFileName);
}

QString ExportDlg::regDataDeclFileName() const
{
    return ui->leDataDecl->text();
}

void ExportDlg::setRegDataDeclFileName(const QString& newRegDataDeclFileName)
{
    ui->leDataImpl->setText(newRegDataDeclFileName);
}

QString ExportDlg::regDataImplFileName() const
{
    return ui->leDataImpl->text();
}

void ExportDlg::setRegDataImplFileName(const QString& newRegDataImplFileName)
{
    ui->leDataImpl->setText(newRegDataImplFileName);
}

QString ExportDlg::cohFileName() const
{
    return ui->leCOH->text();
}

void ExportDlg::setCohFileName(const QString& newCohFileName)
{
    ui->leCOH->setText(newCohFileName);
}

QString ExportDlg::cocFileName() const
{
    return ui->leCOC->text();
}

void ExportDlg::setCocFileName(const QString& newCocFileName)
{
    ui->leCOC->setText(newCocFileName);
}

QString ExportDlg::dataName() const
{
    return ui->leDataName->text();
}

void ExportDlg::setDataName(const QString& newDataName)
{
    ui->leDataName->setText(newDataName);
}

void ExportDlg::on_tbPath_clicked(bool checked)
{
    Q_UNUSED(checked);

    QString path = QFileDialog::getExistingDirectory(this, tr("Выбор папки для экспорта"), ui->lePath->text(), QFileDialog::ShowDirsOnly);

    if(!path.isEmpty()){
        ui->lePath->setText(path);
    }
}

void ExportDlg::on_tbRegIdsSel_clicked(bool checked)
{
    Q_UNUSED(checked);

    selectFileNameTo(ui->leRegIds, tr("Выбор файла идентификаторов"), QStringLiteral("Файлы языка С (*.c *.h)"));
}

void ExportDlg::on_tbRegListSel_clicked(bool checked)
{
    Q_UNUSED(checked);

    selectFileNameTo(ui->leRegList, tr("Выбор файла списка"), QStringLiteral("Файлы языка С (*.c *.h)"));
}

void ExportDlg::on_tbDataDeclSel_clicked(bool checked)
{
    Q_UNUSED(checked);

    selectFileNameTo(ui->leDataDecl, tr("Выбор файла декларации данных"), QStringLiteral("Файлы языка С (*.c *.h)"));
}

void ExportDlg::on_tbDataImplSel_clicked(bool checked)
{
    Q_UNUSED(checked);

    selectFileNameTo(ui->leDataImpl, tr("Выбор файла реализации данных"), QStringLiteral("Файлы языка С (*.c *.h)"));
}

void ExportDlg::on_tbCOHSel_clicked(bool checked)
{
    Q_UNUSED(checked);

    selectFileNameTo(ui->leCOH, tr("Выбор файла декларации словаря"), QStringLiteral("Файлы языка С (*.c *.h)"));
}

void ExportDlg::on_tbCOCSel_clicked(bool checked)
{
    Q_UNUSED(checked);

    selectFileNameTo(ui->leCOC, tr("Выбор файла реализации словаря"), QStringLiteral("Файлы языка С (*.c *.h)"));
}

void ExportDlg::selectFileNameTo(QLineEdit* le, const QString& caption, const QString& filter)
{
    QString curFileName = QDir(ui->lePath->text()).filePath(le->text());

    QString fileName = QFileDialog::getSaveFileName(this, caption, curFileName, filter);

    if(!fileName.isEmpty()){
        le->setText(QFileInfo(fileName).fileName());
    }
}

bool ExportDlg::exportCO() const
{
    return ui->cbExportCO->isChecked();
}

void ExportDlg::setExportCO(bool newExportCO)
{
    ui->cbExportCO->setChecked(newExportCO);
}

bool ExportDlg::exportData() const
{
    return ui->cbExportData->isChecked();
}

void ExportDlg::setExportData(bool newExportData)
{
    ui->cbExportData->setChecked(newExportData);
}

bool ExportDlg::exportRegs() const
{
    return ui->cbExportRegs->isChecked();
}

void ExportDlg::setExportRegs(bool newExportRegs)
{
    ui->cbExportRegs->setChecked(newExportRegs);
}
