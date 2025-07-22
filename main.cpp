#include "candrivewin.h"
#include <QApplication>
#include <QTextCodec>

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

/*#include <QDebug>
#include <QString>
#include "regutils.h"
static void testMakeName()
{
    qDebug() << RegUtils::makeName("Device type", RegUtils::SyntaxType::camelCase);
    qDebug() << RegUtils::makeName("COB-ID SYNC message", RegUtils::SyntaxType::camelCase);
    qDebug() << RegUtils::makeName("COB-ID time stamp object", RegUtils::SyntaxType::camelCase);
    qDebug() << RegUtils::makeName("COB-ID EMCY", RegUtils::SyntaxType::camelCase);
    qDebug() << RegUtils::makeName("Inhibit time EMCY", RegUtils::SyntaxType::camelCase);

    qDebug() << RegUtils::makeName("Device type", RegUtils::SyntaxType::snake_case);
    qDebug() << RegUtils::makeName("COB-ID SYNC message", RegUtils::SyntaxType::snake_case);
    qDebug() << RegUtils::makeName("COB-ID time stamp object", RegUtils::SyntaxType::snake_case);
    qDebug() << RegUtils::makeName("COB-ID EMCY", RegUtils::SyntaxType::snake_case);
    qDebug() << RegUtils::makeName("Inhibit time EMCY", RegUtils::SyntaxType::snake_case);

    qDebug() << RegUtils::makeName("Device type", RegUtils::SyntaxType::UPPER_CASE);
    qDebug() << RegUtils::makeName("COB-ID SYNC message", RegUtils::SyntaxType::UPPER_CASE);
    qDebug() << RegUtils::makeName("COB-ID time stamp object", RegUtils::SyntaxType::UPPER_CASE);
    qDebug() << RegUtils::makeName("COB-ID EMCY", RegUtils::SyntaxType::UPPER_CASE);
    qDebug() << RegUtils::makeName("Inhibit time EMCY", RegUtils::SyntaxType::UPPER_CASE);
}*/

/*#include "regtypes.h"
#include <QString>
#include <QStringList>
#include <QDebug>
static void testGetNames()
{
    reg_flags_t flags = RegFlag::CONF | RegFlag::READONLY;
    auto strlist = RegTypes::getNames(flags, RegTypes::flagName);
    qDebug() << strlist.join("|");
}*/

/*#include <QRegExp>
#include <QDebug>
static void testRegExpReplace()
{
    QRegExp rx("\\$\\([A-Za-z0-9_]*\\)", Qt::CaseSensitive);
    qDebug() << rx.isValid() << rx.errorString();
    qDebug() << rx.exactMatch("$(MY_VAR)");

    QMap<QString, QString> vars = {
        {"REG_DATA", "reg_data"},
        {"OD_NAME", "OD"}
    };

    if(rx.isValid()){
        QString str = "$(REG_DATA) and $(OD_NAME) or $(NONAME)";
        int pos = 0;
        int len = 0;
        while((pos = rx.indexIn(str, 0)) != -1){
            len = rx.matchedLength();
            QString var_name = str.mid(pos+2, len-3);
            qDebug() << var_name;
            str.replace(pos, len, vars.value(var_name));
        }
        qDebug() << str;
    }
}*/


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CanDriveWin w;

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    w.show();

    //testRegExp();
    //testStr();
    //testMakeName();
    //testGetNames();
    //testRegExpReplace();

    return a.exec();
}

