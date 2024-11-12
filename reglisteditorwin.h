#ifndef REGLISTEDITORWIN_H
#define REGLISTEDITORWIN_H

#include <QMainWindow>
#include <QString>

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
class ExportDlg;


class RegListEditorWin : public QMainWindow
{
    Q_OBJECT

public:
    RegListEditorWin(QWidget *parent = nullptr);
    ~RegListEditorWin();

private slots:
    void on_actOpen_triggered(bool checked = false);
    void on_actOpenAppend_triggered(bool checked = false);
    void on_actSaveAs_triggered(bool checked = false);
    void on_actExport_triggered(bool checked = false);
    void on_actQuit_triggered(bool checked = false);
    void on_actExpandTree_triggered(bool checked = false);
    void on_actCollapseTree_triggered(bool checked = false);
    void on_actMoveUp_triggered(bool checked = false);
    void on_actMoveDown_triggered(bool checked = false);
    void on_actAddItem_triggered(bool checked = false);
    void on_actDuplicateItem_triggered(bool checked = false);
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
    ExportDlg* m_exportDlg;

    RegListModel* m_regsListModel;

    RegDelegate* m_regListDelegate;

    QString m_curDir;

    void appendFile(const QString& fileName);

    void doDlgExportRegs();
    void doDlgExportData();
    void doDlgExportCo();
    void doDlgExportEds();
};
#endif // REGLISTEDITORWIN_H
