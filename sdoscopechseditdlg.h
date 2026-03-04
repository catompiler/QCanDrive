#ifndef SDOSCOPECHSEDITDLG_H
#define SDOSCOPECHSEDITDLG_H

#include <QDialog>

namespace Ui {
class SDOScopeChsEditDlg;
}

class SDOScopeChsEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SDOScopeChsEditDlg(QWidget *parent = nullptr);
    ~SDOScopeChsEditDlg();

private:
    Ui::SDOScopeChsEditDlg *ui;
};

#endif // SDOSCOPECHSEDITDLG_H
