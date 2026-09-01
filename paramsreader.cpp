#include "paramsreader.h"
#include <QFile>
#include <QSettings>
#include <QDebug>
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

    QSettings settings(fileName, QSettings::IniFormat);
    if(!settings.isWritable()) return false;

    for(const RegVar* rv: varList){
        const RegEntry* re = rv->parent();
        if(!re){
            qDebug() << "Invalid parent in regvar:" << rv->memAddr();
            continue;
        }

        reg_fullindex_t fullindex = RegUtils::makeFullIndex(re->index(), rv->subIndex());
        if(auto it = valuesList.find(fullindex); it != valuesList.end()){
            int32_t value = (*it);

            if(!writeVar(settings, rv, value)){
                qDebug() << "Error writing regvar:" << rv->memAddr();
                continue;
            }
        }
    }

    return true;
}

ParamsReader::RegValuesList ParamsReader::read(const QString& fileName, const RegVarList& varList) const
{
    qDebug() << "ParamsReader::read";

    RegValuesList valuesList;

    QSettings settings(fileName, QSettings::IniFormat);

    for(const RegVar* rv: varList){
        const RegEntry* re = rv->parent();
        if(!re){
            qDebug() << "Invalid parent in regvar:" << rv->memAddr();
            continue;
        }

        bool isOk = false;
        int32_t value = readVar(settings, rv, &isOk);

        if(isOk){
            reg_fullindex_t fullindex = RegUtils::makeFullIndex(re->index(), rv->subIndex());
            valuesList.insert(fullindex, value);
        }else{
            qDebug() << "Error reading regvar:" << rv->memAddr();
            continue;
        }
    }

    return valuesList;
}

bool ParamsReader::writeVar(QSettings& settings, const RegVar* rv, int32_t value) const
{
    settings.beginGroup(rv->memAddr());

    settings.setValue("type", static_cast<int>(rv->dataType()));
    settings.setValue("value", value);

    settings.endGroup();

    return true;
}

int32_t ParamsReader::readVar(QSettings& settings, const RegVar* rv, bool* isOk) const
{
    settings.beginGroup(rv->memAddr());

    bool ok = false;

    DataType type = static_cast<DataType>(settings.value("type").toInt(&ok));
    if(!ok || type != rv->dataType()){
        if(isOk) *isOk = false;
        return 0;
    }

    int32_t value = settings.value("value").toInt(&ok);
    if(!ok){
        if(isOk) *isOk = false;
        return 0;
    }

    settings.endGroup();

    if(isOk) *isOk = true;
    return value;
}
