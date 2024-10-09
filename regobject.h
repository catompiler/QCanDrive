#ifndef REGOBJECT_H
#define REGOBJECT_H

#include <stdint.h>
#include <QString>
#include "regtypes.h"


/**
 * @brief Базовый класс объектов регистров.
 */
class RegObject
{
public:

    virtual ~RegObject();

    //! Создаёт объект заданного типа.
    static RegObject* newByType(ObjectType type);

    //! Удаляет объект заданного типа.
    static void deleteByType(RegObject* object);

    //! Получает тип.
    virtual ObjectType type() const = 0;

    //! Родитель.
    RegObject* parent() const;

    //! Имя.
    QString name() const;
    void setName(const QString &newName);

    //! Описание.
    QString description() const;
    void setDescription(const QString &newDescription);

protected:
    explicit RegObject(RegObject* parent);
    RegObject(const RegObject& obj);
    void setParent(RegObject* newParent);

    RegObject* m_parent;
    QString m_name;
    QString m_description;
};




#endif // REGOBJECT_H
