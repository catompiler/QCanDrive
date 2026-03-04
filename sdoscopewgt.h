#ifndef SDOSCOPEWGT_H
#define SDOSCOPEWGT_H

#include <QWidget>

class OscPlot;
class SDOScope;
class RegListModel;
class SLCanOpenNode;

namespace Ui {
class SDOScopeWgt;
}

class SDOScopeWgt : public QWidget
{
    Q_OBJECT

public:
    explicit SDOScopeWgt(QWidget *parent = nullptr);
    ~SDOScopeWgt();

    const RegListModel* regListModel() const;
    RegListModel* regListModel();
    void setRegListModel(RegListModel* newRegListModel);

    SLCanOpenNode* getSLCanOpenNode();
    void setSLCanOpenNode(SLCanOpenNode* slcon);

public slots:
    void connected();
    void disconnected();
    void single();

private slots:
    void sdoscopeInitialized();
    void sdoscopeUpdated();
    void sdoscopeErrorOccured();
    void sdoscopeDone();
    void sdoscopeReaded();

private:
    Ui::SDOScopeWgt *ui;

    SDOScope* m_scope;

    bool m_initialized;

    OscPlot* getPlot();
    const OscPlot* getPlot() const;

    void refreshUi();
};

#endif // SDOSCOPEWGT_H
