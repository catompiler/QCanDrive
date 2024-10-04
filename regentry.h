#ifndef REGENTRY_H
#define REGENTRY_H

#include <stdint.h>
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

    //! Получение индекса.
    reg_index_t index() const;
    //! Установка индекса.
    RegEntry& setIndex(reg_index_t index);

    //! Получение типа объекта.
    ObjectType objectType() const;
    //! Установка типа объекта.
    void setObjectType(ObjectType obj_type);

private:
    reg_index_t m_index;
    reg_subindex_t m_subEntreiesCount;
    ObjectType m_object_type;
    RegObject* m_object;
};

#endif // REGENTRY_H
