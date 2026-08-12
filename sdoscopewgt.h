#ifndef SDOSCOPEWGT_H
#define SDOSCOPEWGT_H

#include <QWidget>

class OScopePlot;
class SDOScope;
class SDOScopeData;
class RegListModel;
class SLCanOpenNode;

class SDOScopeChsEditDlg;
class SDOScopeParamsDlg;

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

    void horiScaleChanged(double value);
    void horiOffsetChanged(double value);

    void btnScopeParams_clicked(bool checked);
    void btnScopeChannels_clicked(bool checked);

private:
    Ui::SDOScopeWgt *ui;
    SDOScopeChsEditDlg* m_chs_edit_dlg;
    SDOScopeParamsDlg* m_params_dlg;

    SDOScope* m_scope;
    SDOScopeData* m_scope_data;

    bool m_initialized;

    OScopePlot* getPlot();
    const OScopePlot* getPlot() const;

    void populateChannelsUi();
    void refreshUi();
    void refreshChannelsUi();
    void applyUiToPlot();
};

#endif // SDOSCOPEWGT_H
