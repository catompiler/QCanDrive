#ifndef REGARRAY_H
#define REGARRAY_H

#include "regmultiobject.h"
#include "regvar.h"
#include <QVector>

class RegVar;


class RegArray : public RegMultiObject
{
public:
    RegArray();
    ~RegArray();

    ObjectType type() const override;

    int count() const override;

    bool add(RegObject* regobj) override;
    bool remove(int index) override;

    RegVar* at(int index) override;

private:
    QVector<RegVar*> m_array;

    void updateCounter();
};

#endif // REGARRAY_H
