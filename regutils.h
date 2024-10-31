#ifndef REGUTILS_H
#define REGUTILS_H

#include <stdint.h>
#include "regtypes.h"
#include <QString>
#include <QPair>
#include <QList>
#include <QMap>
#include "reglistmodel.h"

class RegEntry;
class RegVar;

typedef uint32_t reg_fullindex_t;

namespace RegUtils{

// //! Получает систему счисления числа в строке.
//extern int numberBase(const QString& str, bool* ok = nullptr);

//! Получает строковое представление индекса и под-индекса.
extern QString indexSubIndexToString(uint index, uint subindex);

//! Получает индекс и под-индекс из строки.
extern QPair<uint, uint> indexSubIndexFromString(const QString& index_str, bool* ok = nullptr);

//! Получает полный индекс регистра по индексу и под-индексу.
extern reg_fullindex_t makeFullIndex(reg_index_t index, reg_subindex_t subindex);

//! Получает пару индекс + под-индекс из полного индекса.
extern QPair<reg_index_t, reg_subindex_t> getIndexSubIndex(reg_fullindex_t full_index);

//! Типы маппинга имён.
namespace NameMapping{
    enum Value {
        WITHIN_ENTRY = 0,
        WITHIN_ALL = 1
    };
}

//! Генерирует имена для повторяющихся имён точек входа.
extern QMap<reg_index_t, QString> genRegDataEntryNameMapping(const RegEntryList* regentrylist);

//! Генерирует имена для повторяющихся имён переменных.
extern QMap<reg_fullindex_t, QString> genRegDataVarsNameMapping(const RegEntryList* regentrylist, NameMapping::Value mappingType, const QMap<reg_index_t, QString>* entryMapping);

//! Генерирует имена для повторяющихся имён переменных внутри точки входа.
extern QMap<reg_fullindex_t, QString> genRegDataVarsNameMappingWithinEntry(const RegEntryList* regentrylist);

//! Генерирует имена для повторяющихся имён переменных внутри всего списка.
extern QMap<reg_fullindex_t, QString> genRegDataVarsNameMappingWithinAll(const RegEntryList* regentrylist, const QMap<reg_index_t, QString>* entryMapping);

}

#endif // REGUTILS_H
