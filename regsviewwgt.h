#ifndef REGSVIEWWGT_H
#define REGSVIEWWGT_H

#include <QTreeView>
#include <QString>
//#include "reglistmodel.h"
#include "regsviewmodel.h"

class QItemSelection;
class QModelIndex;
class RegListModel;
class RegEntry;
class Settings;


class RegsViewWgt : public QTreeView
{
    Q_OBJECT

public:
    RegsViewWgt(QWidget *parent = nullptr);
    ~RegsViewWgt();

    const RegListModel* regListModel() const;
    RegListModel* regListModel();
    void setRegListModel(RegListModel* newRegListModel);

public slots:
    void expandTree();
    void collapseTree();

private slots:
    void m_tvRegList_activated(const QModelIndex &index);
    void m_tvRegList_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);

private:
    QTreeView* getTreeView() { return this; }

    RegsViewModel* m_regsViewModel;

    void restoreSettings();
    void storeSettings();
};
#endif // REGSVIEWWGT_H
