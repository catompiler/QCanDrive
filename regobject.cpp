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

RegObject* RegObject::byType(ObjectType type)
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
