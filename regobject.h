#ifndef REGOBJECT_H
#define REGOBJECT_H

#include <stdint.h>
#include <QString>
#include "regtypes.h"

class RegEntry;

/**
 * @brief Базовый класс объектов регистров.
 */
class RegObject
{
public:

    explicit RegObject(RegEntry* parent = nullptr);
    RegObject(const RegObject& obj);
    virtual ~RegObject();

    //static RegObject* newByType(ObjectType type);

    //! Получает тип.
    virtual ObjectType type() const = 0;

    //! Родитель.
    RegEntry* parent() const;

    //! Имя.
    QString name() const;
    void setName(const QString &newName);

    //! Описание.
    QString description() const;
    void setDescription(const QString &newDescription);

protected:
    RegEntry* m_parent;
    QString m_name;
    QString m_description;

    void setParent(RegEntry* newParent);
};




#endif // REGOBJECT_H
