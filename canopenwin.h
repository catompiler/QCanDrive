#ifndef CANOPENWIN_H
#define CANOPENWIN_H

#include <QMainWindow>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class CanOpenWin; }
QT_END_NAMESPACE


class QMenu;
class QPoint;
//class QGridLayout;
class SLCanOpenNode;
class CoValuesHolder;
class SDOValue;
class SDOValuePlot;
class TrendPlotEditDlg;
class SignalCurveEditDlg;
class SDOValueDialEditDlg;


class CanOpenWin : public QMainWindow
{
    Q_OBJECT

public:
    CanOpenWin(QWidget *parent = nullptr);
    ~CanOpenWin();

private slots:
    void on_actQuit_triggered(bool checked);
    void on_actDebugExec_triggered(bool checked);
    void on_actConnect_triggered(bool checked);
    void on_actDisconnect_triggered(bool checked);
    void on_actAddPlot_triggered(bool checked);
    void on_actEditPlot_triggered(bool checked);
    void on_actDelPlot_triggered(bool checked);
    void on_actAddDial_triggered(bool checked);
    void on_actEditDial_triggered(bool checked);
    void on_actDelDial_triggered(bool checked);

    void CANopen_connected();
    void CANopen_disconnected();
private:
    Ui::CanOpenWin *ui;
    //SDOValuePlot* m_plot;
    SLCanOpenNode* m_slcon;
    CoValuesHolder* m_valsHolder;
    QGridLayout* m_layout;
    QMenu* m_plotsMenu;
    QMenu* m_dialsMenu;

    int m_updIntervalms;

    TrendPlotEditDlg* m_trendDlg;
    SignalCurveEditDlg* m_signalCurveEditDlg;
    SDOValueDialEditDlg* m_dialDlg;

    template <typename WidgetType>
    WidgetType* findWidgetTypeAt(const QPoint& pos);

    void showPlotContextMenu(const QPoint& pos);
    void showDialsContextMenu(const QPoint& pos);
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
