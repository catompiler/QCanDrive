#include "regobject.h"
#include "regarray.h"
#include "regrecord.h"
#include "regvar.h"
#include <stdexcept>


RegObject::RegObject(RegObject* parent)
{
    m_parent = parent;
}

RegObject::RegObject(const RegObject& obj)
{
    m_parent = obj.m_parent;
    m_name = obj.m_name;
    m_description = obj.m_description;
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

RegObject* RegObject::parent() const
{
    return m_parent;
}

void RegObject::setParent(RegObject* newParent)
{
    m_parent = newParent;
}

QString RegObject::name() const
{
    return m_name;
}

void RegObject::setName(const QString &newName)
{
    m_name = newName;
}

QString RegObject::description() const
{
    return m_description;
}

void RegObject::setDescription(const QString &newDescription)
{
    m_description = newDescription;
}
