#ifndef REGARRAY_H
#define REGARRAY_H

#include "regobject.h"


class RegArray : public RegObject
{
public:
    RegArray();
    ~RegArray();

    // RegObject interface
public:
    ObjectType type() const override;
};

#endif // REGARRAY_H
