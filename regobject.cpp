#include "regobject.h"
#include "regentry.h"
#include "regvar.h"
#include <stdexcept>


RegObject::RegObject(RegEntry* parent)
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

//RegObject* RegObject::newByType(ObjectType type)
//{
//    switch(type){
//    case ObjectType::VAR:
//        return new RegVar();
//    case ObjectType::ARR:
//    case ObjectType::REC:
//        return new RegEntry(type);
//    }
//    return nullptr;
//}

RegEntry* RegObject::parent() const
{
    return m_parent;
}

void RegObject::setParent(RegEntry* newParent)
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
