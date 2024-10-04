#ifndef REGLISTEDITORWIN_H
#define REGLISTEDITORWIN_H

#include <QMainWindow>
#include <QList>


QT_BEGIN_NAMESPACE
namespace Ui { class RegListEditorWin; }
QT_END_NAMESPACE

class RegListModel;
class RegEntry;


class RegListEditorWin : public QMainWindow
{
    Q_OBJECT

public:
    RegListEditorWin(QWidget *parent = nullptr);
    ~RegListEditorWin();

public slots:
    void on_pbAdd_clicked();

private:
    Ui::RegListEditorWin *ui;
    RegListModel* m_regsListModel;

    QList<RegEntry*>* m_reg_entries;
};
#endif // REGLISTEDITORWIN_H
