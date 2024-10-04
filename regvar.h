#ifndef REGVAR_H
#define REGVAR_H

#include <QVariant>
#include "regobject.h"


class RegVar : public RegObject
{
public:
    RegVar();
    ~RegVar();

    // RegObject interface
public:
    ObjectType type() const override;

private:
    QVariant m_value;
};

#endif // REGVAR_H
