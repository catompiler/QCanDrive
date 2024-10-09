#include "regmultiobject.h"
#include "regvar.h"


RegMultiObject::RegMultiObject(RegObject* parent)
    :RegObject(parent)
{
    m_var_count = new RegVar(this);
    m_var_count->setDataType(DataType::UNSIGNED8);
    m_var_count->setMinValue(0);
    m_var_count->setMaxValue(254);
    m_var_count->setDefaultValue(0);
    m_var_count->setName(QObject::tr("count"));
    m_var_count->setDescription(QObject::tr("Count of subitems"));
}

RegMultiObject::~RegMultiObject()
{
    delete m_var_count;
}
