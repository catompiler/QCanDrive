#ifndef SDOSCOPEWGT_H
#define SDOSCOPEWGT_H

#include <QWidget>

class OScopePlot;
class SDOScope;
class RegListModel;
class SLCanOpenNode;

class SDOScopeChsEditDlg;

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

    void on_tbChannels_clicked(bool checked);

private:
    Ui::SDOScopeWgt *ui;
    SDOScopeChsEditDlg* m_chs_edit_dlg;

    SDOScope* m_scope;

    bool m_initialized;

    OScopePlot* getPlot();
    const OScopePlot* getPlot() const;

    void refreshUi();
};

#endif // SDOSCOPEWGT_H
