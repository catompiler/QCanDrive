#include "regentry.h"



RegEntry::RegEntry(reg_index_t index, ObjectType type)
{
    m_index = index;
    m_object_type = type;
}

RegEntry::~RegEntry()
{

}

reg_index_t RegEntry::index() const
{
    return m_index;
}

RegEntry& RegEntry::setIndex(reg_index_t index)
{
    m_index = index;

    return *this;
}

ObjectType RegEntry::objectType() const
{
    return m_object_type;
}

void RegEntry::setObjectType(ObjectType obj_type)
{
    m_object_type = obj_type;
}

