#ifndef REGLISTEDITORWIN_H
#define REGLISTEDITORWIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class RegListEditorWin; }
QT_END_NAMESPACE

class RegListModel;


class RegListEditorWin : public QMainWindow
{
    Q_OBJECT

public:
    RegListEditorWin(QWidget *parent = nullptr);
    ~RegListEditorWin();

private:
    Ui::RegListEditorWin *ui;
    RegListModel* regsListModel;
};
#endif // REGLISTEDITORWIN_H
