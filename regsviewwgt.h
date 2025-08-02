#ifndef REGSVIEWWGT_H
#define REGSVIEWWGT_H

#include <QTreeView>
#include <QString>
//#include "reglistmodel.h"
#include "regsviewmodel.h"
#include "cotypes.h"

class QItemSelection;
class QModelIndex;
class QTimer;
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

    SLCanOpenNode* getSLCanOpenNode();
    const SLCanOpenNode* getSLCanOpenNode() const;
    void setSLCanOpenNode(SLCanOpenNode* slcon);

    CO::NodeId nodeId() const;
    void setNodeId(CO::NodeId newNodeId);

    uint regsRefreshPeriod() const;
    void setRegsRefreshPeriod(uint newRegsRefreshPeriod);

public slots:
    void expandTree();
    void collapseTree();
    void refreshRegs();
    void setRefreshingRegs(bool newRefreshing);

private slots:
    void m_tvRegList_activated(const QModelIndex &index);
    void m_tvRegList_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);

private:
    QTreeView* getTreeView() { return this; }

    RegsViewModel* m_regsViewModel;

    QTimer* m_refreshTimer;

    void restoreSettings();
    void storeSettings();
};
#endif // REGSVIEWWGT_H
