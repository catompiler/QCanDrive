#ifndef CANOPENWIN_H
#define CANOPENWIN_H

#include <QMainWindow>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class CanOpenWin; }
QT_END_NAMESPACE


class QMenu;
class QPoint;
class Settings;
class SettingsDlg;
class SLCanOpenNode;
class CoValuesHolder;
class SDOValue;
class SDOValuePlot;
class TrendPlotEditDlg;
class SignalCurveEditDlg;
class SDOValueDialEditDlg;
class SDOValueSliderEditDlg;
class SDOValueBarEditDlg;
class SDOValueButtonEditDlg;
class SDOValueIndicatorEditDlg;


class CanOpenWin : public QMainWindow
{
    Q_OBJECT

public:
    CanOpenWin(QWidget *parent = nullptr);
    ~CanOpenWin();

private slots:
    void on_actQuit_triggered(bool checked);
    void on_actDebugExec_triggered(bool checked);
    void on_actSettings_triggered(bool checked);
    void on_actConnect_triggered(bool checked);
    void on_actDisconnect_triggered(bool checked);
    void on_actAddPlot_triggered(bool checked);
    void on_actEditPlot_triggered(bool checked);
    void on_actDelPlot_triggered(bool checked);
    void on_actAddDial_triggered(bool checked);
    void on_actEditDial_triggered(bool checked);
    void on_actDelDial_triggered(bool checked);
    void on_actAddSlider_triggered(bool checked);
    void on_actEditSlider_triggered(bool checked);
    void on_actDelSlider_triggered(bool checked);
    void on_actAddBar_triggered(bool checked);
    void on_actEditBar_triggered(bool checked);
    void on_actDelBar_triggered(bool checked);
    void on_actAddButton_triggered(bool checked);
    void on_actEditButton_triggered(bool checked);
    void on_actDelButton_triggered(bool checked);
    void on_actAddIndicator_triggered(bool checked);
    void on_actEditIndicator_triggered(bool checked);
    void on_actDelIndicator_triggered(bool checked);

    void CANopen_connected();
    void CANopen_disconnected();
private:
    Ui::CanOpenWin *ui;
    //SDOValuePlot* m_plot;
    SLCanOpenNode* m_slcon;
    CoValuesHolder* m_valsHolder;
    QGridLayout* m_layout;
    QMenu* m_cockpitMenu;
    QMenu* m_plotsMenu;
    QMenu* m_dialsMenu;
    QMenu* m_slidersMenu;
    QMenu* m_barsMenu;
    QMenu* m_buttonsMenu;
    QMenu* m_indicatorsMenu;

    int m_updIntervalms;

    Settings* m_settings;
    SettingsDlg* m_settingsDlg;
    TrendPlotEditDlg* m_trendDlg;
    SignalCurveEditDlg* m_signalCurveEditDlg;
    SDOValueDialEditDlg* m_dialDlg;
    SDOValueSliderEditDlg* m_sliderDlg;
    SDOValueBarEditDlg* m_barDlg;
    SDOValueButtonEditDlg* m_buttonDlg;
    SDOValueIndicatorEditDlg* m_indicatorDlg;

    void applySettings();

    template <typename WidgetType>
    WidgetType* findWidgetTypeAt(const QPoint& pos);

    void showPlotContextMenu(const QPoint& pos);
    void showDialsContextMenu(const QPoint& pos);
    void showSlidersContextMenu(const QPoint& pos);
    void showBarsContextMenu(const QPoint& pos);
    void showButtonsContextMenu(const QPoint& pos);
    void showIndicatorsContextMenu(const QPoint& pos);
};

template <typename WidgetType>
WidgetType* CanOpenWin::findWidgetTypeAt(const QPoint& pos)
{
    WidgetType* wt = nullptr;

    for(int i = 0; i < m_layout->count(); i ++){

        auto item = m_layout->itemAt(i);

        if(QWidget* wgt = item->widget()){

            if(wgt->rect().contains(wgt->mapFromParent(pos))){

                wt = qobject_cast<WidgetType*>(wgt);
                if(wt != nullptr) break;
            }
        }
    }

    return wt;
}

#endif // CANOPENWIN_H
