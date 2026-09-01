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
class RegsViewDelegate;
class RegEntry;
class Settings;
class QProgressDialog;
class ParamsLoader;
class ParamsReader;


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
    void uploadFromDrive();
    void downloadToDrive();

private slots:
    void m_tvRegList_activated(const QModelIndex &index);
    void m_tvRegList_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);

    void m_paramsLoader_errorOccured(const QString& errStr);
    void m_paramsLoader_done();
    void m_paramsLoader_finished();
    void m_paramsLoader_progressChanged(int progress);
    void m_progressDialog_canceled();

private:
    QTreeView* getTreeView() { return this; }

    RegsViewModel* m_regsViewModel;

    RegsViewDelegate* m_regsViewDelegate;

    QTimer* m_refreshTimer;

    QProgressDialog* m_progressDlg;
    ParamsLoader* m_paramsLoader;
    ParamsReader* m_paramsReader;

    void restoreSettings();
    void storeSettings();
};
#endif // REGSVIEWWGT_H
