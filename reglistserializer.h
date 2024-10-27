#ifndef REGLISTSERIALIZER_H
#define REGLISTSERIALIZER_H

#include <QObject>
#include <QVector>
#include "reglistmodel.h"

class QIODevice;
class RegEntry;
class RegVar;


class RegListSerializer : public QObject
{
    Q_OBJECT
public:
    explicit RegListSerializer(QObject *parent = nullptr);
    virtual ~RegListSerializer();

    virtual bool serialize(QIODevice* dev, const RegEntryList* reglist) = 0;
    virtual bool deserialize(QIODevice* dev, RegEntryList* reglist) = 0;

signals:

};

#endif // REGLISTSERIALIZER_H
