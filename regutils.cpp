#include "regutils.h"



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
