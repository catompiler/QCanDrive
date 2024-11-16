#include "canopenwin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CanOpenWin w;
    w.show();
    return a.exec();
}
