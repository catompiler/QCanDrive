#ifndef CANOPENWIN_H
#define CANOPENWIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CanOpenWin; }
QT_END_NAMESPACE

class CanOpenWin : public QMainWindow
{
    Q_OBJECT

public:
    CanOpenWin(QWidget *parent = nullptr);
    ~CanOpenWin();

private:
    Ui::CanOpenWin *ui;
};
#endif // CANOPENWIN_H
