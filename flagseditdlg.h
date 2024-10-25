#ifndef FLAGSEDITDLG_H
#define FLAGSEDITDLG_H

#include <QDialog>

namespace Ui {
class FlagsEditDlg;
}

class FlagsEditModel;

class FlagsEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FlagsEditDlg(QWidget *parent = nullptr);
    ~FlagsEditDlg();

    QStringList flagsNames() const;
    void setFlagsNames(const QStringList& newFlagsNames);

    uint flagsValues() const;
    void setFlagsValues(uint newFlagsValues);

private:
    Ui::FlagsEditDlg *ui;
    FlagsEditModel* m_flagsEditModel;
};

#endif // FLAGSEDITDLG_H
