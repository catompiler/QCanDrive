#include "regobject.h"
#include "regarray.h"
#include "regrecord.h"
#include "regvar.h"
#include <stdexcept>


RegObject::RegObject()
{

}

RegObject::~RegObject()
{

}

RegObject* RegObject::newByType(ObjectType type)
{
    switch(type){
    default:
        break;
    case ObjectType::VAR:
        return new RegVar();
    case ObjectType::ARR:
        return new RegArray();
    case ObjectType::REC:
        return new RegRecord();
    }

    throw std::runtime_error("Invalid reg object type");
}

void RegObject::deleteByType(RegObject* object)
{
    if(object == nullptr) return;

    ObjectType type = object->type();

    switch(type){
    default:
        throw std::runtime_error("Invalid reg object type");
        break;
    case ObjectType::VAR:
        delete object;
        break;
    case ObjectType::ARR:
        delete object;
        break;
    case ObjectType::REC:
        delete object;
        break;
    }
}
