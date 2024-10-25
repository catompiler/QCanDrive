#ifndef REGSELECTDLG_H
#define REGSELECTDLG_H

#include <QDialog>
#include <QPair>


namespace Ui {
class RegSelectDlg;
}

class RegListModel;
class RegSelectModel;
class QItemSelection;


class RegSelectDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RegSelectDlg(QWidget *parent = nullptr);
    ~RegSelectDlg();

    RegListModel* regListModel() const;
    void setRegListModel(RegListModel* newRegListModel);

    bool hasSelectedReg() const;
    QPair<uint, uint> selectedRegIndex() const;
    void selectReg(uint index, uint subIndex);

    // QWidget interface
protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void regList_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    Ui::RegSelectDlg *ui;
    RegSelectModel* m_regSelMdl;
};

#endif // REGSELECTDLG_H
