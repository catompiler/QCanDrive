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

/*#include <QString>
#include <QDebug>
static void testStr()
{
    QString src = "0x1234";
    qDebug() << src;
    QString index_str = src.section(QChar('.'), 0, 0);
    QString sub_index_str = src.section(QChar('.'), 1, 1);
    qDebug() << index_str << sub_index_str;
    bool ok_index = false, ok_sub_index = false;
    unsigned int index = index_str.toUInt(&ok_index, 16);
    unsigned int sub_index = sub_index_str.toUInt(&ok_sub_index, 16);
    qDebug() << ok_index << index << ok_sub_index << sub_index;
}*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    
    w.show();

    //testRegExp();
    //testStr();

    return a.exec();
}

