#include "regutils.h"
#include "regentry.h"
#include "regvar.h"
#include <QDebug>



QString RegUtils::indexSubIndexToString(uint index, uint subindex)
{
    return QString("0x%1.%2")
            .arg(index, 4, 16, QChar('0'))
            .arg(subindex, 2, 16, QChar('0'));
}

QPair<uint, uint> RegUtils::indexSubIndexFromString(const QString& index_str, bool* ok)
{
    int base = 10;

    QString str_val = index_str;

    if(str_val.startsWith("0x")){
        str_val = str_val.mid(2);
        base = 16;
    }else if(str_val.startsWith("0b")){
        str_val = str_val.mid(2);
        base = 2;
    }else if(str_val.startsWith("0")){
        base = 8;
    }

    bool ok_index = false, ok_subindex = false;
    uint index = str_val.section(QChar('.'), 0, 0).toUInt(&ok_index, base);
    uint subindex = str_val.section(QChar('.'), 1, 1).toUInt(&ok_subindex, base);

    if(ok) *ok = ok_index && ok_subindex;

    return qMakePair(index, subindex);
}

reg_fullindex_t RegUtils::makeFullIndex(reg_index_t index, reg_subindex_t subindex)
{
    return (static_cast<reg_fullindex_t>(index) << 8) |
           (static_cast<reg_fullindex_t>(subindex) & 0xff);
}

QPair<reg_index_t, reg_subindex_t> RegUtils::getIndexSubIndex(reg_fullindex_t full_index)
{
    reg_index_t index = full_index >> 8;
    reg_subindex_t subindex = full_index & 0xff;

    return qMakePair(index, subindex);
}

QMap<reg_index_t, QString> RegUtils::genRegDataEntryNameMapping(const RegEntryList* regentrylist)
{
//    qDebug() << "genRegDataEntryNameMapping";

    QMap<reg_index_t, QString> res;
    QSet<QString> names;

    for(auto reit = regentrylist->cbegin(); reit != regentrylist->cend(); ++ reit){
        RegEntry* re = *reit;

//        qDebug() << Qt::showbase << Qt::hex << re->index() << re->name();

        QString entryName = re->name();

        if(names.contains(entryName)){
            entryName = QString("%1_%2").arg(re->name(), QString::number(re->index(), 16));
            for(uint i = 1; names.contains(entryName); i ++){
                entryName = QString("%1_%2_%3").arg(re->name(), QString::number(re->index(), 16), QString::number(i, 10));
            }
            res.insert(re->index(), entryName);
        }
        names.insert(entryName);
    }

//    // DEBUG.
//    for(auto it = res.begin(); it != res.end(); ++ it){
//        qDebug () << Qt::hex << Qt::showbase << it.key() << it.value();
//    }

    return res;
}

QMap<reg_fullindex_t, QString> RegUtils::genRegDataVarsNameMapping(const RegEntryList* regentrylist)
{
    return genRegDataVarsNameMappingWithinEntry(regentrylist);
}

QMap<reg_fullindex_t, QString> RegUtils::genRegDataVarsNameMappingWithinEntry(const RegEntryList* regentrylist)
{
//    qDebug() << "genRegDataVarsNameMappingWithinEntry";

    QMap<reg_fullindex_t, QString> res;
    QSet<QString> names;

    for(auto reit = regentrylist->cbegin(); reit != regentrylist->cend(); ++ reit){
        RegEntry* re = *reit;
        for(auto rvit = re->cbegin(); rvit != re->cend(); ++ rvit){
            RegVar* rv = *rvit;

            reg_fullindex_t fullindex = makeFullIndex(re->index(), rv->subIndex());

//            qDebug() << Qt::showbase << Qt::hex << fullindex
//                     << re->name() << rv->name()
//                     << Qt::noshowbase << Qt::dec << rv->count();

            QString varName = rv->name();

            if(names.contains(varName)){
                varName = QString("%1_%2").arg(rv->name(), QString::number(rv->subIndex(), 10));
                for(uint i = 1; names.contains(varName); i ++){
                    varName = QString("%1_%2_%3").arg(rv->name(), QString::number(fullindex, 16), QString::number(i, 10));
                }
                res.insert(fullindex, varName);
            }
            names.insert(varName);
        }
        names.clear();
    }

//    // DEBUG.
//    for(auto it = res.begin(); it != res.end(); ++ it){
//        qDebug () << Qt::hex << Qt::showbase << it.key() << it.value();
//    }

    return res;
}

QString RegUtils::getEntryName(const RegEntry* re, const EntryNameMap* entryMapping)
{
    if(entryMapping){
        auto it = entryMapping->find(re->index());
        if(it != entryMapping->cend()){
            return it.value();
        }
    }
    return re->name();
}

QString RegUtils::getVarName(const RegEntry* re, const RegVar* rv, const VarNameMap* varMapping)
{
    if(varMapping){
        auto it = varMapping->find(makeFullIndex(re->index(), rv->subIndex()));
        if(it != varMapping->cend()){
            return it.value();
        }
    }
    return rv->name();
}

QString RegUtils::getVarDecl(const RegEntry* re, const RegVar* rv, const VarNameMap* varMapping)
{
    QString name = getVarName(re, rv, varMapping);

    if(rv->count() > 1){
        name = QStringLiteral("%1[%2]").arg(name).arg(rv->count());
    }

    return name;
}

QString RegUtils::getVarMem(const QString& name, const RegEntry* re, const RegVar* rv, uint index, const EntryNameMap* entryMapping, const VarNameMap* varMapping)
{
    if(!rv->memAddr().isEmpty()){
        if(rv->count() <= 1) return rv->memAddr();
        return rv->memAddr(index);
    }

    QString entryName = getEntryName(re, entryMapping);
    QString varName = getVarName(re, rv, varMapping);

    QString mem = QStringLiteral("%1.%2").arg(entryName, varName);

    if(rv->count() > 1){
        mem = QStringLiteral("%1[%2]").arg(mem).arg(index);
    }

    if(!name.isEmpty()){
        mem = QStringLiteral("%1.%2").arg(name, mem);
    }

    return mem;
}

/*
RegUtils::AutoDataMem RegUtils::genAutoData(const QString& name, const RegEntryList* reglist, const EntryNameMap* entryMapping, const VarNameMap* varMapping)
{
    AutoDataMem automem;

    automem.name = name;

    std::for_each(reglist->begin(), reglist->end(), [&](const RegEntry* re){

        QString entryName = getEntryName(re, entryMapping);
        auto entryData = automem.entries[re->index()];
        entryData.name = entryName;

        std::for_each(re->cbegin(), re->cend(), [&](const RegVar* rv){

            QString varName = getVarName(re, rv, varMapping);
            auto varData = entryData.vars[rv->subIndex()];
            varData.name = varName;
        });
    });

    return automem;
}
*/

QString RegUtils::getVarDefValData(const RegVar* rv)
{
    QString data;

    QString defval = rv->defaultValue().toString();

    if(rv->count() <= 1){
        data = defval;
    }else{
        data += QStringLiteral("{ ");
        for(uint i = 0; i < rv->count(); i ++){
            if(i != 0){
                data += QStringLiteral(", ");
            }

            data += defval;
        }
        data += QStringLiteral(" }");
    }

    return data;
}

QString RegUtils::makeStructTypeName(const QString& name)
{
    return QStringLiteral("_S_%1").arg(name);
}
