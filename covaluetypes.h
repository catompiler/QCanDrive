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
#include "regtypes.h"



namespace COValue
{

//enum Type {
//    I32 = 0,
//    I16 = 1,
//    I8 = 2,
//    U32 = 3,
//    U16 = 4,
//    U8 = 5,
//    IQ24 = 6,
//    IQ15 = 7,
//    IQ7 = 8,
//    STR = 9,
//    MEM = 10
//};
using Type = ::DataType;

// for compatibility.
const ::DataType I32  = ::DataType::I32;
const ::DataType I16  = ::DataType::I16;
const ::DataType I8   = ::DataType::I8;
const ::DataType U32  = ::DataType::U32;
const ::DataType U16  = ::DataType::U16;
const ::DataType U8   = ::DataType::U8;
const ::DataType IQ24 = ::DataType::IQ24;
const ::DataType IQ15 = ::DataType::IQ15;
const ::DataType IQ7  = ::DataType::IQ7;
const ::DataType STR  = ::DataType::STR;
const ::DataType MEM  = ::DataType::MEM;


extern QList<QPair<QString, Type>> getTypesNames();


template <int N, typename ET, ET first>
inline constexpr int FindEnumElement(const ET target)
{
    return (target == first) ? N : -1;
}

template <int N, typename ET, ET first, ET next, ET... other>
inline constexpr int FindEnumElement(const ET target)
{
    return (target == first) ? N : FindEnumElement<N + 1, ET, next, other...>(target);
}

template <typename ET>
constexpr size_t typeSize(const ET target, const int defSize = 0)
{
    const auto sizes = std::array{4, 2, 1, 4, 2, 1, 4, 4, 4, 0, 0};

    const int N = FindEnumElement<0, ET, Type::I32, Type::I16, Type::I8,
                                         Type::U32, Type::U16, Type::U8,
                                         Type::IQ24, Type::IQ15, Type::IQ7,
                                         Type::STR, Type::MEM>(target);

    return (N >= 0) ? sizes[N] : defSize;
}


template <typename ValType>
ValType valueFrom(const void* valueData, const std::variant<COValue::Type, size_t> typeOrSize, const ValType& defVal = ValType())
{
    const auto pType = std::get_if<COValue::Type>(&typeOrSize);
    const auto pSize = std::get_if<size_t>(&typeOrSize);

    if(pType){
        switch(*pType){
        default:
            break;
        case Type::I32:
            return ValType(*static_cast<const int32_t*>(valueData));
        case Type::I16:
            return ValType(*static_cast<const int16_t*>(valueData));
        case Type::I8:
            return ValType(*static_cast<const int8_t*>(valueData));
        case Type::U32:
            return ValType(*static_cast<const uint32_t*>(valueData));
        case Type::U16:
            return ValType(*static_cast<const uint16_t*>(valueData));
        case Type::U8:
            return ValType(*static_cast<const uint8_t*>(valueData));
        case Type::IQ24:
            return ValType(*static_cast<const int32_t*>(valueData)) / (1<<24);
        case Type::IQ15:
            return ValType(*static_cast<const int32_t*>(valueData)) / (1<<15);
        case Type::IQ7:
            return ValType(*static_cast<const int32_t*>(valueData)) / (1<<7);
        case Type::STR:
        case Type::MEM:
            break;
        }
    }else if(pSize){
        if(*pSize == sizeof(ValType)){
            return *static_cast<const ValType*>(valueData);
        }
    }

    return defVal;
}


template <typename ValType>
bool valueTo(void* valueData, const std::variant<COValue::Type, size_t> typeOrSize, const ValType& val)
{
    const auto pType = std::get_if<COValue::Type>(&typeOrSize);
    const auto pSize = std::get_if<size_t>(&typeOrSize);

    if(pType){
        switch(*pType){
        default:
            return false;
        case Type::I32:
            *static_cast<int32_t*>(valueData) = static_cast<int32_t>(val);
            break;
        case Type::I16:
            *static_cast<int16_t*>(valueData) = static_cast<int16_t>(val);
            break;
        case Type::I8:
            *static_cast<int8_t*>(valueData) = static_cast<int8_t>(val);
            break;
        case Type::U32:
            *static_cast<uint32_t*>(valueData) = static_cast<uint32_t>(val);
            break;
        case Type::U16:
            *static_cast<uint16_t*>(valueData) = static_cast<uint16_t>(val);
            break;
        case Type::U8:
            *static_cast<uint8_t*>(valueData) = static_cast<uint8_t>(val);
            break;
        case Type::IQ24:
            *static_cast<int32_t*>(valueData) = static_cast<int32_t>(val * (1<<24));
            break;
        case Type::IQ15:
            *static_cast<int32_t*>(valueData) = static_cast<int32_t>(val * (1<<15));
            break;
        case Type::IQ7:
            *static_cast<int32_t*>(valueData) = static_cast<int32_t>(val * (1<<7));
            break;
        case Type::STR:
        case Type::MEM:
            return false;
        }
    }else if(pSize){
        if(*pSize == sizeof(ValType)){
            *static_cast<ValType*>(valueData) = val;
        }else{
            return false;
        }
    }else{
        return false;
    }
    return true;
}

}


#endif // COVALUETYPES_H
