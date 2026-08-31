#include "paramsreader.h"
#include <QFile>
#include "regentry.h"
#include "regvar.h"


ParamsReader::ParamsReader(QObject *parent)
    : QObject{parent}
{
}

ParamsReader::~ParamsReader()
{
}

bool ParamsReader::write(const QString& fileName, const RegVarList& varList, const RegValuesList valuesList) const
{
    qDebug() << "ParamsReader::write";

    QFile file(fileName);
    if(!file.isWritable()) return false;

    for(const RegVar* rv: varList){
        qDebug() << rv->memAddr();
    }

    return true;
}

ParamsReader::RegValuesList ParamsReader::read(const QString& fileName, const RegVarList& varList) const
{
    qDebug() << "ParamsReader::read";

    RegValuesList valuesList;

    QFile file(fileName);
    if(!file.isReadable()) return valuesList;

    for(const RegVar* rv: varList){
        qDebug() << rv->memAddr();
    }

    return valuesList;
}
