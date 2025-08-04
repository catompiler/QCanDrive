#ifndef COCKPITWGT_H
#define COCKPITWGT_H

#include <QWidget>
#include <QAction>
#include <QGridLayout>
#include "cockpitserializer.h"


QT_BEGIN_NAMESPACE
namespace Ui { class CockpitWgt; }
QT_END_NAMESPACE


class QMenu;
class QPoint;
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


class CockpitWgt : public QWidget
{
    Q_OBJECT

public:
    CockpitWgt(QWidget *parent = nullptr);
    ~CockpitWgt();

    CoValuesHolder* valsHolder() const;
    void setValsHolder(CoValuesHolder* newValsHolder);

    void openCockpitFile(const QString& fileName);

public slots:
    void saveCockpit();
    void openCockpit();
    void clearCockpit();

    void addPlot();
    void addDial();
    void addSlider();
    void addBar();
    void addButton();
    void addIndicator();

private slots:
    void m_actAddPlot_triggered(bool checked);
    void m_actEditPlot_triggered(bool checked);
    void m_actDelPlot_triggered(bool checked);
    void m_actAddDial_triggered(bool checked);
    void m_actEditDial_triggered(bool checked);
    void m_actDelDial_triggered(bool checked);
    void m_actAddSlider_triggered(bool checked);
    void m_actEditSlider_triggered(bool checked);
    void m_actDelSlider_triggered(bool checked);
    void m_actAddBar_triggered(bool checked);
    void m_actEditBar_triggered(bool checked);
    void m_actDelBar_triggered(bool checked);
    void m_actAddButton_triggered(bool checked);
    void m_actEditButton_triggered(bool checked);
    void m_actDelButton_triggered(bool checked);
    void m_actAddIndicator_triggered(bool checked);
    void m_actEditIndicator_triggered(bool checked);
    void m_actDelIndicator_triggered(bool checked);

private:
    CoValuesHolder* m_valsHolder;

    QGridLayout* m_layout;
    QMenu* m_cockpitMenu;
    QAction* m_actAddPlot;
    QAction* m_actAddDial;
    QAction* m_actAddSlider;
    QAction* m_actAddBar;
    QAction* m_actAddButton;
    QAction* m_actAddIndicator;
    QMenu* m_plotsMenu;
    QAction* m_actEditPlot;
    QAction* m_actDelPlot;
    QMenu* m_dialsMenu;
    QAction* m_actEditDial;
    QAction* m_actDelDial;
    QMenu* m_slidersMenu;
    QAction* m_actEditSlider;
    QAction* m_actDelSlider;
    QMenu* m_barsMenu;
    QAction* m_actEditBar;
    QAction* m_actDelBar;
    QMenu* m_buttonsMenu;
    QAction* m_actEditButton;
    QAction* m_actDelButton;
    QMenu* m_indicatorsMenu;
    QAction* m_actEditIndicator;
    QAction* m_actDelIndicator;

    TrendPlotEditDlg* m_trendDlg;
    SignalCurveEditDlg* m_signalCurveEditDlg;
    SDOValueDialEditDlg* m_dialDlg;
    SDOValueSliderEditDlg* m_sliderDlg;
    SDOValueBarEditDlg* m_barDlg;
    SDOValueButtonEditDlg* m_buttonDlg;
    SDOValueIndicatorEditDlg* m_indicatorDlg;

    void createMenus();
    void destroyMenus();

    void applySettings();
    void clearCockpitWidgets();
    CockpitSerializer::CockpitWidgets collectCockpitWidgets();
    void appendCockpitWidgets(const CockpitSerializer::CockpitWidgets& widgets);

    template <typename WidgetType>
    WidgetType* findWidgetTypeAt(const QPoint& pos);

    void showPlotsContextMenu(const QPoint& pos);
    void showDialsContextMenu(const QPoint& pos);
    void showSlidersContextMenu(const QPoint& pos);
    void showBarsContextMenu(const QPoint& pos);
    void showButtonsContextMenu(const QPoint& pos);
    void showIndicatorsContextMenu(const QPoint& pos);

    inline QWidget* centralWidget() { return this; }
};

template <typename WidgetType>
WidgetType* CockpitWgt::findWidgetTypeAt(const QPoint& pos)
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

#endif // COCKPITWGT_H
