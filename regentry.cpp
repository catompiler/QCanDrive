#include "regentry.h"
#include "regvar.h"
#include <algorithm>


RegEntry::RegEntry()
    :RegObject()
{
    m_index = 0;
    m_type = ObjectType::VAR;
}

RegEntry::RegEntry(ObjectType type)
    :RegObject()
{
    m_index = 0;
    m_type = type;
}

RegEntry::RegEntry(reg_index_t index, ObjectType type)
    :RegObject()
{
    m_index = index;
    m_type = type;
}

RegEntry::RegEntry(const RegEntry& entry)
    :RegObject(entry)
{
    m_index = entry.m_index;
    m_type = entry.m_type;

    for(auto& orig_rv: entry.m_vars){
        RegVar* rv = new RegVar(*orig_rv);
        rv->setParent(this);
        m_vars.append(rv);
    }
}

RegEntry::~RegEntry()
{
    qDeleteAll(m_vars);
}

reg_index_t RegEntry::index() const
{
    return m_index;
}

void RegEntry::setIndex(reg_index_t index)
{
    m_index = index;
}

ObjectType RegEntry::type() const
{
    return m_type;
}

void RegEntry::setType(ObjectType type)
{
    m_type = type;
}

bool RegEntry::add(RegVar* var)
{
    if(var == nullptr) return false;
    if(find(var) != -1) return false;
    if(var->parent() != nullptr && var->parent() != this) return false;

    var->setParent(this);
    m_vars.append(var);

    return true;
}

bool RegEntry::remove(int index)
{
    if(index >= m_vars.count()) return false;

    auto var = m_vars.at(index);

    m_vars.remove(index);

    delete var;

    return true;
}

RegVar* RegEntry::at(int index) const
{
    if(index >= m_vars.count()) return nullptr;

    return m_vars.at(index);
}

int RegEntry::find(RegVar* var) const
{
    auto it = std::find(m_vars.begin(), m_vars.end(), var);

    if(it == m_vars.end()) return -1;

    return std::distance(m_vars.begin(), it);
}

int RegEntry::count() const
{
    return m_vars.count();
}

int RegEntry::countAll() const
{
    int count = 0;
    std::for_each(cbegin(), cend(), [&count](const RegVar* rv){
        count += rv->count();
    });
    return count;
}

RegEntry::VarListIterator RegEntry::begin()
{
    return m_vars.begin();
}

RegEntry::ConstVarListIterator RegEntry::begin() const
{
    return m_vars.begin();
}

RegEntry::ConstVarListIterator RegEntry::cbegin() const
{
    return m_vars.cbegin();
}

RegEntry::VarListIterator RegEntry::end()
{
    return m_vars.end();
}

RegEntry::ConstVarListIterator RegEntry::end() const
{
    return m_vars.end();
}

RegEntry::ConstVarListIterator RegEntry::cend() const
{
    return m_vars.cend();
}

bool RegEntry::hasVarBySubIndex(reg_subindex_t subIndex) const
{
    return varBySubIndex(subIndex) != nullptr;
}

RegVar* RegEntry::varBySubIndex(reg_subindex_t subIndex) const
{
    auto it = std::find_if(cbegin(), cend(), [subIndex](const RegVar* r){
        return r->subIndex() == subIndex;
    });
    if(it == cend()) return nullptr;

    return *it;
}

reg_subindex_t RegEntry::lastVarSubIndex() const
{
    if(m_vars.isEmpty()) return 0;
    return m_vars.last()->subIndex();
}

