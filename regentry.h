#ifndef REGENTRY_H
#define REGENTRY_H

#include <stdint.h>
#include <QVector>
#include "regtypes.h"
#include "regobject.h"

class RegVar;


/**
 * @brief The RegEntry class.
 */
class RegEntry:
    public RegObject
{
public:

    typedef QVector<RegVar*> VarList;
    typedef VarList::iterator VarListIterator;
    typedef VarList::const_iterator ConstVarListIterator;

    RegEntry();
    explicit RegEntry(ObjectType type);
    explicit RegEntry(reg_index_t index, ObjectType type);
    RegEntry(const RegEntry& entry) = delete;
    ~RegEntry();

    //! Индекс.
    reg_index_t index() const;
    void setIndex(reg_index_t index);

    //! Тип объекта.
    ObjectType type() const;
    void setType(ObjectType type);

    bool add(RegVar* var);
    bool remove(int index);
    RegVar* at(int index) const;
    int find(RegVar* var) const;
    int count() const;

    VarListIterator begin();
    ConstVarListIterator cbegin() const;
    VarListIterator end();
    ConstVarListIterator cend() const;

    bool hasVarBySubIndex(reg_subindex_t subIndex) const;
    RegVar* varBySubIndex(reg_subindex_t subIndex) const;

private:
    reg_index_t m_index;
    ObjectType m_type;

    QVector<RegVar*> m_vars;
};

#endif // REGENTRY_H
