#ifndef REGMULTIOBJECT_H
#define REGMULTIOBJECT_H

#include "regobject.h"

class RegVar;


class RegMultiObject : public RegObject
{
public:
    ~RegMultiObject();

    virtual int count() const = 0;

    virtual bool add(RegObject* regobj) = 0;
    virtual bool remove(int index) = 0;

    virtual RegObject* at(int index) = 0;

    RegVar* countVariable() const;

protected:
    explicit RegMultiObject(RegObject* parent);
    RegMultiObject(const RegMultiObject& obj) = delete;

    RegVar* m_var_count;
};

#endif // REGMULTIOBJECT_H
