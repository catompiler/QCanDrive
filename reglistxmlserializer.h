#ifndef REGLISTXMLSERIALIZER_H
#define REGLISTXMLSERIALIZER_H

#include "reglistserializer.h"

class QXmlStreamWriter;
class QXmlStreamReader;
class RegEntry;
class RegVar;


class RegListXmlSerializer : public RegListSerializer
{
    Q_OBJECT
public:
    explicit RegListXmlSerializer(QObject *parent = nullptr);
    ~RegListXmlSerializer();

    // RegListSerializer interface
public:
    bool serialize(QIODevice* dev, const RegEntryList* reglist) override;
    bool deserialize(QIODevice* dev, RegEntryList* reglist) override;

private:
    bool writeRegs(QXmlStreamWriter* writer, const RegEntryList* reglist) const;
    bool writeRegEntry(QXmlStreamWriter* writer, const RegEntry* re) const;
    bool writeRegVars(QXmlStreamWriter* writer, const RegEntry* re) const;
    bool writeRegVar(QXmlStreamWriter* writer, const RegVar* rv) const;
    bool readRegs(QXmlStreamReader* reader, RegEntryList* reglist) const;
    bool readRegEntry(QXmlStreamReader* reader, RegEntry* re) const;
    bool readRegVars(QXmlStreamReader* reader, RegEntry* re) const;
    bool readRegVar(QXmlStreamReader* reader, RegVar* rv) const;
};

#endif // REGLISTXMLSERIALIZER_H
