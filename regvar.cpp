#include "regvar.h"

RegVar::RegVar()
{

}

RegVar::~RegVar()
{

}


ObjectType RegVar::type() const
{
    return ObjectType::VAR;
}
