#ifndef REGLISTXML2SERIALIZER_H
#define REGLISTXML2SERIALIZER_H

#include "reglistserializer.h"

class QXmlStreamWriter;
class QXmlStreamReader;
class RegEntry;
class RegVar;


class RegListXml2Serializer : public RegListSerializer
{
    Q_OBJECT
public:
    explicit RegListXml2Serializer(QObject *parent = nullptr);
    ~RegListXml2Serializer();

    // RegListSerializer interface
public:
    bool serialize(QIODevice* dev, const RegEntryList* reglist) override;
    bool deserialize(QIODevice* dev, RegEntryList* reglist) override;

protected:

    int intValue(const QStringRef& str, int defVal = 0) const;
    int intValue(const QString& str, int defVal = 0) const;
    unsigned int uintValue(const QStringRef& str, unsigned int defVal = 0) const;
    unsigned int uintValue(const QString& str, unsigned int defVal = 0) const;
    double realValue(const QStringRef& str, double defVal = 0) const;
    double realValue(const QString& str, double defVal = 0) const;

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

#endif // REGLISTXML2SERIALIZER_H
