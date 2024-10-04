#ifndef REGRECORD_H
#define REGRECORD_H

#include "regobject.h"


class RegRecord : public RegObject
{
public:
    RegRecord();
    ~RegRecord();

    // RegObject interface
public:
    ObjectType type() const override;
};

#endif // REGRECORD_H
