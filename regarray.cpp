#include "regarray.h"
#include "regvar.h"



RegArray::RegArray()
    :RegMultiObject(nullptr)
{
    updateCounter();
}

RegArray::~RegArray()
{
    for(auto var: qAsConst(m_array)){
        delete var;
    }
    m_array.clear();
}


ObjectType RegArray::type() const
{
    return ObjectType::ARR;
}

int RegArray::count() const
{
    return m_array.count();
}

bool RegArray::add(RegObject* regobj)
{
    RegVar* item = nullptr;

    if(regobj == nullptr){
        if(!m_array.isEmpty()){
            RegVar* refItem = m_array.first();

            item = new RegVar(refItem);
        }else{
            item = new RegVar(this);
        }
    }else if(regobj->type() != ObjectType::VAR){
        return false;
    }else{
        item = static_cast<RegVar*>(regobj);
        item->setParent(this);
    }

    m_array.append(item);

    updateCounter();

    return true;
}

bool RegArray::remove(int index)
{
    if(index >= m_array.count()) return false;

    m_array.removeAt(index);

    updateCounter();

    return true;
}

RegVar* RegArray::at(int index)
{
    return m_array.at(index);
}

void RegArray::updateCounter()
{
    m_var_count->setDefaultValue(m_array.count());
}
