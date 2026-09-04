#include "stringutils.h"
#include <QString>
#include <QStringView>


namespace StringUtils{


int intValue(const QStringView& str, int defVal)
{
    bool ok = false;
    decltype(defVal) val = str.toInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

int intValue(const QString& str, int defVal)
{
    bool ok = false;
    decltype(defVal) val = str.toInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

unsigned int uintValue(const QStringView& str, unsigned int defVal)
{
    bool ok = false;
    decltype(defVal) val = str.toUInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

unsigned int uintValue(const QString& str, unsigned int defVal)
{
    bool ok = false;
    decltype(defVal) val = str.toUInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

double realValue(const QStringView& str, double defVal)
{
    bool ok = false;
    decltype(defVal) val = str.toDouble(&ok);
    if(ok) return val;
    return defVal;
}

double realValue(const QString& str, double defVal)
{
    bool ok = false;
    decltype(defVal) val = str.toDouble(&ok);
    if(ok) return val;
    return defVal;
}

}
