#ifndef CANOPENWIN_H
#define CANOPENWIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CanOpenWin; }
QT_END_NAMESPACE


class SLCanOpenNode;


class CanOpenWin : public QMainWindow
{
    Q_OBJECT

public:
    CanOpenWin(QWidget *parent = nullptr);
    ~CanOpenWin();

private slots:
    void on_actDebugExec_triggered(bool checked);

private:
    Ui::CanOpenWin *ui;

    SLCanOpenNode* m_sco;
};
#endif // CANOPENWIN_H
