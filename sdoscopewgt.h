#ifndef SDOSCOPEWGT_H
#define SDOSCOPEWGT_H

#include <QWidget>

class OScopePlot;
class SDOScope;
class SDOScopeData;
class RegListModel;
class RegSelectDlg;
class SLCanOpenNode;

class SDOScopeChsEditDlg;
class SDOScopeParamsDlg;
class QTimer;

namespace Ui {
class SDOScopeWgt;
}

class SDOScopeWgt : public QWidget
{
    Q_OBJECT

public:

    static constexpr int TRIGGER_VALUE_APPLY_DELAY_MS = 500;

    explicit SDOScopeWgt(QWidget *parent = nullptr);
    ~SDOScopeWgt();

    const RegListModel* regListModel() const;
    RegListModel* regListModel();
    void setRegListModel(RegListModel* newRegListModel);

    SLCanOpenNode* getSLCanOpenNode();
    void setSLCanOpenNode(SLCanOpenNode* slcon);

    RegSelectDlg* regSelectDialog() const;
    void setRegSelectDialog(RegSelectDlg* newRegSelectDialog);

public slots:
    void connected();
    void disconnected();
    void single();
    void run();
    void stopRun();
    void abort();
    void autoScale();

private slots:
    void sdoscopeFinished();
    void sdoscopeInitialized();
    void sdoscopeUpdated();
    void sdoscopeErrorOccured();
    void sdoscopeDone();
    void sdoscopeReaded();
    void sdoscopeApplied();
    void sdoscopeAppliedChannels();
    void sdoscopeAppliedCommon();
    void sdoscopeAppliedTrig();

    void horiScaleChanged(double value);
    void horiOffsetChanged(double value);

    void triggerEnabledChanged(bool newEnabled);
    void triggerTypeChanged(int newType /* SDOScope::TriggerType */);
    void triggerValueChanged(qreal newValue);
    void triggerChannelChanged(uint newChannel);
    void triggerValueChangedTmr_timeout();

    void cursorsFloatingEnabledChanged(bool newEnabled);

    void btnScopeParams_clicked(bool checked);
    void btnScopeChannels_clicked(bool checked);
    void btnRun_clicked(bool checked);
    void btnSingle_clicked(bool checked);
    void btnAbort_clicked(bool checked);
    void btnAutoScale_clicked(bool checked);

private:
    Ui::SDOScopeWgt *ui;
    SDOScopeChsEditDlg* m_chs_edit_dlg;
    SDOScopeParamsDlg* m_params_dlg;

    QTimer* m_triggerValueChangedTmr;

    SDOScope* m_scope;
    SDOScopeData* m_scope_data;

    bool m_initialized;

    OScopePlot* getPlot();
    const OScopePlot* getPlot() const;

    void populateChannelsUi();
    void populateTriggerChannels();
    void updateUiEnabled();
    void refreshUi();
    void refreshChannelsUi();
    void refreshTriggerUi();
    void refreshCursorsUi();
    void updateTriggerUiByChannel();
    void applyUiToPlot();
    void applyHoriUiToPlot();
    void applyChannelsUiToPlot();
    void applyTriggerUiToPlot();
    void applyCursorsUiToPlot();
};

#endif // SDOSCOPEWGT_H
