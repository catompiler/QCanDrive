#include "mainwindow.h"
#include <QApplication>

/*
#include <QRegExp>
#include <QDebug>
static void testRegExp()
{
    QRegExp rx("^[-+]?[1-9]+[0-9]*$", Qt::CaseInsensitive);
    qDebug() << rx.isValid() << rx.errorString();
    qDebug() << rx.exactMatch("-8174383");
}
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    
    w.show();

    //testRegExp();

    return a.exec();
}

