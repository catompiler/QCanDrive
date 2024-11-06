#include "regutils.h"
#include "regentry.h"
#include "regvar.h"
#include <algorithm>
#include <functional>
#include <QRegExp>
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

            QString varName;

            if(re->type() == ObjectType::ARR && rv->subIndex() != 0x0){
                varName = re->name();
            }else{
                varName = rv->name();
            }

            if(names.contains(varName)){
                varName = QString("%1_%2").arg(rv->name(), QString::number(rv->subIndex(), 10));
                for(uint i = 1; names.contains(varName); i ++){
                    varName = QString("%1_%2_%3").arg(rv->name(), QString::number(fullindex, 16), QString::number(i, 10));
                }
                res.insert(fullindex, varName);
            }

            if(re->type() == ObjectType::ARR && rv->subIndex() != 0x0){
                res.insert(fullindex, varName);
            }else{
                names.insert(varName);
            }
        }
        names.clear();
    }

//    // DEBUG.
//    for(auto it = res.begin(); it != res.end(); ++ it){
//        qDebug () << Qt::hex << Qt::showbase << it.key() << it.value();
//    }

    return res;
}

uint RegUtils::getArrDataLen(const RegEntry* re)
{
    uint count = 0;

    for(auto rvit = re->cbegin(); rvit != re->cend(); ++ rvit){
        RegVar* rv = *rvit;

        if(rv->subIndex() != 0){

            //count = qMax(count, static_cast<uint>(rv->subIndex() - 1));
            count += qMax(1U, rv->count());
        }
    }

    return count;
}

uint RegUtils::getArrDataIndex(const RegEntry* re, const RegVar* rv, uint index)
{
    uint res_index = 0;

    for(auto rvit = re->cbegin(); rvit != re->cend(); ++ rvit){
        RegVar* cur_rv = *rvit;

        if(cur_rv->subIndex() != 0){

            if(cur_rv == rv){
                res_index = res_index + qMin(index, qMax(1U, cur_rv->count()) - 1U);
                break;
            }

            //res_index = qMax(res_index, static_cast<uint>(cur_rv->subIndex() - 1));
            res_index += qMax(1U, cur_rv->count());
        }
    }

    return res_index;
}

QString RegUtils::getArrName(const RegEntry* re, const RegVar* rv, const VarNameMap* varMapping, SyntaxType syntaxType)
{
    if(varMapping){
        auto it = varMapping->find(makeFullIndex(re->index(), rv->subIndex()));
        if(it != varMapping->cend()){
            return makeName(it.value(), syntaxType);
        }
    }
    return makeName(re->name(), syntaxType);
}

QString RegUtils::getArrDecl(const RegEntry* re, const RegVar* rv, const VarNameMap* varMapping, SyntaxType syntaxType)
{
    QString name = getArrName(re, rv, varMapping, syntaxType);

    name = QStringLiteral("%1[%2]").arg(name).arg(getArrDataLen(re));

    return name;
}

QString RegUtils::getArrMem(const QString& name, const RegEntry* re, const RegVar* rv, uint index, const EntryNameMap* entryMapping, const VarNameMap* varMapping, SyntaxType syntaxType)
{
    uint arr_index = getArrDataIndex(re, rv, index);

    if(!rv->memAddr().isEmpty()){
        //if(rv->count() <= 1) return rv->memAddr();
        return rv->memAddr(arr_index);
    }

    QString entryName = getEntryName(re, entryMapping, syntaxType);
    QString varName = getArrName(re, rv, varMapping, syntaxType);

    QString mem = QStringLiteral("%1.%2[%3]").arg(entryName, varName).arg(arr_index);

    if(!name.isEmpty()){
        mem = QStringLiteral("%1.%2").arg(name, mem);
    }

    return mem;
}

QString RegUtils::getArrDefValData(const RegEntry* re)
{
    QString data;

    QString defval;

    data += QStringLiteral("{ ");

    bool firstVar = true;
    for(auto reit = re->cbegin(); reit != re->cend(); ++ reit){
        const RegVar* rv = *reit;

        if(rv->subIndex() == 0) continue;

        if(!firstVar){
            data += QStringLiteral(", ");
        }
        firstVar = false;

        defval.clear();
        for(uint i = 0; i < rv->count(); i ++){
            if(i != 0){
                defval += QStringLiteral(", ");
            }
            defval += rv->defaultValue().toString();
        }
        data += defval;
    }

    data += QStringLiteral(" }");

    return data;
}

QString RegUtils::makeName(const QString& text, SyntaxType syntaxType)
{
    QString name;
    QStringList words;

    //qDebug() << text;

    words = text.split(QRegExp("\\s+"));
    //qDebug() << words;

    QRegExp re_non_word("\\W");
    std::for_each(words.begin(), words.end(), [&re_non_word](QString& word){
        word.replace(re_non_word, QChar('_'));
    });
    //qDebug() << words;

    QVector<bool> is_abbrev(words.count(), false);
    std::transform(words.begin(), words.end(), is_abbrev.begin(), isAbbreviation);
    //qDebug() << is_abbrev;

    for(int i = 0; i < words.count(); i ++){
        QString& word = words[i];

        switch(syntaxType){
        case SyntaxType::snake_case:
            word = word.toLower();
            break;
        case SyntaxType::camelCase:
            if(!is_abbrev[i]){
                word = word.toLower();
                if(i != 0){
                    if(!word.isEmpty()){
                        word[0] = word[0].toUpper();
                    }
                }
            }else{
                if(i < words.count() - 1){
                    if(is_abbrev[i + 1])
                        word.append('_');
                }
            }
            break;
        case SyntaxType::UPPER_CASE:
            word = word.toUpper();
            break;
        }
    }

    if(syntaxType == SyntaxType::snake_case || syntaxType == SyntaxType::UPPER_CASE){
        name = words.join('_');
    }else if(syntaxType == SyntaxType::camelCase){
        name = words.join("");
    }

    return name;
}

QString RegUtils::getEntryName(const RegEntry* re, const EntryNameMap* entryMapping, SyntaxType syntaxType)
{
    if(entryMapping){
        auto it = entryMapping->find(re->index());
        if(it != entryMapping->cend()){
            return makeName(it.value(), syntaxType);
        }
    }
    return makeName(re->name(), syntaxType);
}

QString RegUtils::getVarName(const RegEntry* re, const RegVar* rv, const VarNameMap* varMapping, SyntaxType syntaxType)
{
    if(varMapping){
        auto it = varMapping->find(makeFullIndex(re->index(), rv->subIndex()));
        if(it != varMapping->cend()){
            return makeName(it.value(), syntaxType);
        }
    }
    return makeName(rv->name(), syntaxType);
}

QString RegUtils::getVarDecl(const RegEntry* re, const RegVar* rv, const VarNameMap* varMapping, SyntaxType syntaxType)
{
    QString name = getVarName(re, rv, varMapping, syntaxType);

    if(rv->count() > 1){
        name = QStringLiteral("%1[%2]").arg(name).arg(rv->count());
    }

    return name;
}

QString RegUtils::getVarMem(const QString& name, const RegEntry* re, const RegVar* rv, uint index, const EntryNameMap* entryMapping, const VarNameMap* varMapping, SyntaxType syntaxType)
{
    if(!rv->memAddr().isEmpty()){
        if(rv->count() <= 1) return rv->memAddr();
        return rv->memAddr(index);
    }

    QString entryName = getEntryName(re, entryMapping, syntaxType);
    QString varName = getVarName(re, rv, varMapping, syntaxType);

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

bool RegUtils::isAbbreviation(const QString& name)
{
    for(auto& c: name){
        if(c.isLetter()){
            if(c.isLower()) return false;
        }else if(!c.isDigit() && c != '_'){
            return false;
        }
    }

    return true;
}
