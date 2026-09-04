#ifndef STRINGUTILS_H
#define STRINGUTILS_H


class QString;
class QStringView;


namespace StringUtils{

int intValue(const QStringView& str, int defVal = 0);
int intValue(const QString& str, int defVal = 0);
unsigned int uintValue(const QStringView& str, unsigned int defVal = 0);
unsigned int uintValue(const QString& str, unsigned int defVal = 0);
double realValue(const QStringView& str, double defVal = 0);
double realValue(const QString& str, double defVal = 0);

}


#endif // STRINGUTILS_H
