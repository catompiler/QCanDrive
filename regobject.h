#ifndef REGOBJECT_H
#define REGOBJECT_H

#include <stdint.h>
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

    //! Получает тип данных.
    virtual DataType dataType() const {};

protected:
    RegObject();
};




#endif // REGOBJECT_H
