#ifndef REGSVIEWWGT_H
#define REGSVIEWWGT_H

#include <QTreeView>
#include <QString>
#include "reglistmodel.h"
#include "regsviewmodel.h"

class QItemSelection;
class QModelIndex;
class RegEntryDlg;
class RegListModel;
class RegDelegate;
class RegEntry;
class RegSelectDlg;
class FlagsEditDlg;
class ExportDlg;
class Settings;


class RegsViewWgt : public QTreeView
{
    Q_OBJECT

public:
    RegsViewWgt(QWidget *parent = nullptr);
    ~RegsViewWgt();

public slots:
    void openRegList();
    void appendRegList();
    void saveAsRegList();
    void expandTree();
    void collapseTree();
    void moveUp();
    void moveDown();
    void addItem();
    void duplicateItem();
    void addSubItem();
    void delItem();
    void delAll();

private slots:
    void m_tvRegList_activated(const QModelIndex &index);
    void m_tvRegList_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);

private:
    QTreeView* getTreeView() { return this; }

    RegEntryDlg* m_regEntryDlg;
    RegSelectDlg* m_regSelectDlg;
    FlagsEditDlg* m_flagsEditDlg;
    ExportDlg* m_exportDlg;

    RegListModel* m_regsViewModel;

    RegDelegate* m_regListDelegate;

    QString m_curDir;

    void appendFile(const QString& fileName);

    void restoreSettings();
    void storeSettings();
};
#endif // REGSVIEWWGT_H
