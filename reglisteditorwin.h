#ifndef REGLISTEDITORWIN_H
#define REGLISTEDITORWIN_H

#include <QMainWindow>
#include "reglistmodel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class RegListEditorWin; }
QT_END_NAMESPACE

class RegEntryDlg;


class RegListEditorWin : public QMainWindow
{
    Q_OBJECT

public:
    RegListEditorWin(QWidget *parent = nullptr);
    ~RegListEditorWin();

public slots:
    void on_pbAdd_clicked();
    void on_pbDel_clicked();

private:
    Ui::RegListEditorWin *ui;
    RegEntryDlg* m_regEntryDlg;

    RegListModel* m_regsListModel;
};
#endif // REGLISTEDITORWIN_H
