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


//! Тип маппинга имён точек входа.
using EntryNameMap = QMap<reg_index_t, QString>;
//! Тип маппинга имён переменных.
using VarNameMap = QMap<reg_fullindex_t, QString>;

//! Генерирует имена для повторяющихся имён точек входа.
extern EntryNameMap genRegDataEntryNameMapping(const RegEntryList* regentrylist);

//! Генерирует имена для повторяющихся имён переменных.
extern VarNameMap genRegDataVarsNameMapping(const RegEntryList* regentrylist);

//! Генерирует имена для повторяющихся имён переменных внутри точки входа.
extern VarNameMap genRegDataVarsNameMappingWithinEntry(const RegEntryList* regentrylist);

enum class SyntaxType {
    snake_case = 0,
    camelCase = 1,
    UPPER_CASE = 2
};

//! Создаёт имя переменной из текста.
extern QString makeName(const QString& text, SyntaxType syntaxType = SyntaxType::camelCase);

//! Получает имя точки входа.
extern QString getEntryName(const RegEntry* re, const EntryNameMap* entryMapping, SyntaxType syntaxType = SyntaxType::camelCase);

//! Получает имя переменной.
extern QString getVarName(const RegEntry* re, const RegVar* rv, const VarNameMap* varMapping, SyntaxType syntaxType = SyntaxType::camelCase);

//! Получает имя переменной для декларации.
extern QString getVarDecl(const RegEntry* re, const RegVar* rv, const VarNameMap* varMapping, SyntaxType syntaxType = SyntaxType::camelCase);

//! Получает память данных.
extern QString getVarMem(const QString& name, const RegEntry* re, const RegVar* rv, uint index, const EntryNameMap* entryMapping, const VarNameMap* varMapping, SyntaxType syntaxType = SyntaxType::camelCase);

//! Получает инициализатор данных по-умолчанию.
extern QString getVarDefValData(const RegVar* rv);

//! Получает имя типа структуры по имени данных.
extern QString makeStructTypeName(const QString& name);

//! Проверяет, является ли текст/слово аббревиатурой (полностью заглавные буквы).
extern bool isAbbreviation(const QString& name);

/*
//! Автоматические данные.
struct AutoDataMem {
    QString name; //!< Имя общей структуры.

    //! Данные точки входа.
    struct EntryData {
        QString name; //!< Имя структуры точки входа.

        //! Данные переменной.
        struct VarData {
            QString name; //!< Имя переменной.
        };

        //! Переменные.
        QHash<reg_subindex_t, VarData> vars;
    };

    //! Точки входа.
    QHash<reg_index_t, EntryData> entries;
};

//! Генерирует автоматические данные.
extern AutoDataMem genAutoData(const QString& name, const RegEntryList* reglist, const EntryNameMap* entryMapping, const VarNameMap* varMapping);
*/

// //! Получает данные переменной.
// extern QString getVarMem(const RegEntry* re, const RegVar* rv, )

//! Получает
}

#endif // REGUTILS_H
