#ifndef EXPORTDLG_H
#define EXPORTDLG_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QRegExp;

namespace Ui {
class ExportDlg;
}

class ExportDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDlg(QWidget *parent = nullptr);
    ~ExportDlg();

    QString path() const;
    void setPath(const QString& newPath);

    QString regIdsFileName() const;
    void setRegIdsFileName(const QString& newRegIdsFileName);

    QString regListFileName() const;
    void setRegListFileName(const QString& newRegListFileName);

    QString regDataDeclFileName() const;
    QString regDataDeclFileNameRaw() const;
    void setRegDataDeclFileName(const QString& newRegDataDeclFileName);

    QString regDataImplFileName() const;
    QString regDataImplFileNameRaw() const;
    void setRegDataImplFileName(const QString& newRegDataImplFileName);

    QString cohFileName() const;
    QString cohFileNameRaw() const;
    void setCohFileName(const QString& newCohFileName);

    QString cocFileName() const;
    QString cocFileNameRaw() const;
    void setCocFileName(const QString& newCocFileName);

    QString edsFileName() const;
    void setEdsFileName(const QString& newEdsFileName);

    QString dataName() const;
    void setDataName(const QString& newDataName);

    QString odName() const;
    void setOdName(const QString& newOdName);

    bool exportRegs() const;
    void setExportRegs(bool newExportRegs);

    bool exportData() const;
    void setExportData(bool newExportData);

    bool exportCO() const;
    void setExportCO(bool newExportCO);

    bool exportEds() const;
    void setExportEds(bool newExportEds);

    QString userCodeRegIds() const;
    void setUserCodeRegIds(const QString& newUserCodeRegIds);

    QString userCodeRegList() const;
    void setUserCodeRegList(const QString& newUserCodeRegList);

    QString userCodeDataDecl() const;
    void setUserCodeDataDecl(const QString& newUserCodeDataDecl);

    QString userCodeDataImpl() const;
    void setUserCodeDataImpl(const QString& newUserCodeDataImpl);

    QString userCodeCOh() const;
    void setUserCodeCOh(const QString& newUserCodeCOh);

    QString userCodeCOc() const;
    void setUserCodeCOc(const QString& newUserCodeCOc);

private slots:
    void on_tbPath_clicked(bool checked = false);
    void on_tbRegIdsSel_clicked(bool checked = false);
    void on_tbRegListSel_clicked(bool checked = false);
    void on_tbDataDeclSel_clicked(bool checked = false);
    void on_tbDataImplSel_clicked(bool checked = false);
    void on_tbCOHSel_clicked(bool checked = false);
    void on_tbCOCSel_clicked(bool checked = false);
    void on_tbEdsSel_clicked(bool checked = false);

private:
    Ui::ExportDlg *ui;

    QRegExp m_varRE;

    void selectFileNameTo(QLineEdit* le, const QString& caption, const QString& filter);
    QString replaceVars(const QString& text) const;
};

#endif // EXPORTDLG_H
