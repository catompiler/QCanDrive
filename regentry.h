#ifndef REGENTRY_H
#define REGENTRY_H

#include <stdint.h>
#include <QString>
#include "regtypes.h"
#include "regobject.h"

/**
 * @brief The RegEntry class.
 */
class RegEntry
{
public:
    explicit RegEntry(reg_index_t index, ObjectType type);
    ~RegEntry();

    //! Индекс.
    reg_index_t index() const;
    void setIndex(reg_index_t index);

    //! Тип объекта.
    ObjectType objectType() const;
    void setObjectType(ObjectType obj_type);

    //! Имя.
    QString name() const;
    void setName(const QString &newName);

    //! Описание.
    QString description() const;
    void setDescription(const QString &newDescription);

    RegObject* object() const;

private:
    reg_index_t m_index;
    reg_subindex_t m_subEntriesCount;
    QString m_name;
    QString m_description;
    RegObject* m_object;
};

#endif // REGENTRY_H
