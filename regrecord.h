#ifndef REGRECORD_H
#define REGRECORD_H

#include "regmultiobject.h"
#include "regvar.h"
#include <QVector>

class RegVar;


class RegRecord : public RegMultiObject
{
public:
    RegRecord();
    ~RegRecord();

    ObjectType type() const override;

    int count() const override;

    bool add(RegObject* regobj) override;
    bool remove(int index) override;

    RegVar* at(int index) override;

private:
    QVector<RegVar*> m_fields;

    void updateCounter();
};

#endif // REGRECORD_H
