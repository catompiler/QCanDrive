#include "regentry.h"



RegEntry::RegEntry(reg_index_t index, ObjectType type)
{
    m_index = index;
    m_object = RegObject::newByType(type);
}

RegEntry::~RegEntry()
{
    RegObject::deleteByType(m_object);
}

reg_index_t RegEntry::index() const
{
    return m_index;
}

void RegEntry::setIndex(reg_index_t index)
{
    m_index = index;
}

ObjectType RegEntry::objectType() const
{
    return m_object->type();
}

void RegEntry::setObjectType(ObjectType obj_type)
{
    if(m_object->type() != obj_type){
        RegObject::deleteByType(m_object);
        m_object = RegObject::newByType(obj_type);
    }
}

QString RegEntry::name() const
{
    return m_name;
}

void RegEntry::setName(const QString &newName)
{
    m_name = newName;
}

QString RegEntry::description() const
{
    return m_description;
}

void RegEntry::setDescription(const QString &newDescription)
{
    m_description = newDescription;
}

RegObject* RegEntry::object() const
{
    return m_object;
}

