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
        RegObject* new_object = RegObject::newByType(obj_type);
        new_object->setName(m_object->name());
        new_object->setDescription(m_object->description());

        RegObject::deleteByType(m_object);
        m_object = new_object;
    }
}

RegObject* RegEntry::object() const
{
    return m_object;
}

