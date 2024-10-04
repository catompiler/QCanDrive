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
    static RegObject* byType(ObjectType type);

    //! Получает тип.
    virtual ObjectType type() const {};

    //! Получает тип данных.
    virtual DataType dataType() const {};

protected:
    RegObject();
};




#endif // REGOBJECT_H
