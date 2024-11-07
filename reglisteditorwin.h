#ifndef REGLISTEDITORWIN_H
#define REGLISTEDITORWIN_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class RegListEditorWin; }
QT_END_NAMESPACE

class QItemSelection;
class QModelIndex;
class RegEntryDlg;
class RegListModel;
class RegDelegate;
class RegEntry;
class RegSelectDlg;
class FlagsEditDlg;


class RegListEditorWin : public QMainWindow
{
    Q_OBJECT

public:
    RegListEditorWin(QWidget *parent = nullptr);
    ~RegListEditorWin();

public slots:
    void on_actOpen_triggered(bool checked = false);
    void on_actOpenAppend_triggered(bool checked = false);
    void on_actSaveAs_triggered(bool checked = false);
    void on_actExportData_triggered(bool checked = false);
    void on_actExportRegs_triggered(bool checked = false);
    void on_actExportCo_triggered(bool checked = false);
    void on_actQuit_triggered(bool checked = false);
    void on_actAddItem_triggered(bool checked = false);
    void on_actAddSubItem_triggered(bool checked = false);
    void on_actDelItem_triggered(bool checked = false);
    void on_actDelAll_triggered(bool checked = false);
    void on_actDebugExec_triggered(bool checked = false);
    void on_tvRegList_activated(const QModelIndex &index);
    void tvRegList_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);

private:
    Ui::RegListEditorWin *ui;
    RegEntryDlg* m_regEntryDlg;
    RegSelectDlg* m_regSelectDlg;
    FlagsEditDlg* m_flagsEditDlg;

    RegListModel* m_regsListModel;

    RegDelegate* m_regListDelegate;
};
#endif // REGLISTEDITORWIN_H
