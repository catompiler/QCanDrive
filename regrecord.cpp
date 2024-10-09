#include "regrecord.h"
#include "regvar.h"



RegRecord::RegRecord()
    :RegMultiObject(nullptr)
{
    updateCounter();
}

RegRecord::~RegRecord()
{
    for(auto var: qAsConst(m_fields)){
        delete var;
    }
    m_fields.clear();
}


ObjectType RegRecord::type() const
{
    return ObjectType::REC;
}

int RegRecord::count() const
{
    return m_fields.count();
}

bool RegRecord::add(RegObject* regobj)
{
    RegVar* item = nullptr;

    if(regobj == nullptr){
        if(!m_fields.isEmpty()){
            RegVar* refItem = m_fields.last();

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

    m_fields.append(item);

    updateCounter();

    return true;
}

bool RegRecord::remove(int index)
{
    if(index >= m_fields.count()) return false;

    m_fields.removeAt(index);

    updateCounter();

    return true;
}

RegVar* RegRecord::at(int index)
{
    return m_fields.at(index);
}

void RegRecord::updateCounter()
{
    m_var_count->setDefaultValue(m_fields.count());
}
