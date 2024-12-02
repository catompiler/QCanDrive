#ifndef COVALUETYPES_H
#define COVALUETYPES_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <array>
#include <utility>
#include <variant>
#include <QPair>
#include <QString>
#include <QList>



namespace COValue
{

enum Type {
    I32 = 0,
    I16 = 1,
    I8 = 2,
    U32 = 3,
    U16 = 4,
    U8 = 5,
    IQ24 = 6,
    IQ15 = 7,
    IQ7 = 8,
    STR = 9,
    MEM = 10
};


extern QList<QPair<QString, Type>> getTypesNames();


template <int N, typename ET, ET first, ET next, ET... other>
inline constexpr int FindEnumElement(const ET target)
{
    if(target == first){
        return N;
    }else{
        return FindEnumElement<N + 1, ET, next, other...>(target);
    }
}

template <int N, typename ET, ET first>
inline constexpr int FindEnumElement(const ET target)
{
    if(target == first){
        return N;
    }else{
        return -1;
    }
}

template <typename ET>
constexpr size_t typeSize(const ET target, const int defSize = 0)
{
    const auto sizes = std::array{4  , 2  , 1 , 4  , 2  , 1 , 4   , 4   , 4  , 0  , 0  };

    const int N = FindEnumElement<0, ET, I32, I16, I8, U32, U16, U8, IQ24, IQ15, IQ7, STR, MEM>(target);

    return (N >= 0) ? sizes[N] : defSize;
}


template <typename Type>
Type valueAs(const void* valueData, const std::variant<COValue::Type, size_t> typeOrSize, const Type& defVal = Type())
{
    const auto pType = std::get_if<COValue::Type>(&typeOrSize);
    const auto pSize = std::get_if<size_t>(&typeOrSize);

    if(pType){
        switch(*pType){
        default:
            break;
        case I32:
            return Type(*static_cast<const int32_t*>(valueData));
        case I16:
            return Type(*static_cast<const int16_t*>(valueData));
        case I8:
            return Type(*static_cast<const int8_t*>(valueData));
        case U32:
            return Type(*static_cast<const uint32_t*>(valueData));
        case U16:
            return Type(*static_cast<const uint16_t*>(valueData));
        case U8:
            return Type(*static_cast<const uint8_t*>(valueData));
        case IQ24:
            return Type(*static_cast<const int32_t*>(valueData)) / (1<<24);
        case IQ15:
            return Type(*static_cast<const int32_t*>(valueData)) / (1<<15);
        case IQ7:
            return Type(*static_cast<const int32_t*>(valueData)) / (1<<7);
        case STR:
            break;
        case MEM:
            return *static_cast<const Type*>(valueData);
        }
    }else if(pSize){
        if(*pSize == sizeof(Type)){
            return *static_cast<const Type*>(valueData);
        }
    }

    return defVal;
}

}


#endif // COVALUETYPES_H
