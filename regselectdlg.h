#ifndef REGSELECTDLG_H
#define REGSELECTDLG_H

#include <QDialog>


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

private slots:
    void regList_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void regList_currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    Ui::RegSelectDlg *ui;
    RegSelectModel* m_regSelMdl;
};

#endif // REGSELECTDLG_H
