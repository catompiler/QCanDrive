#include "reglistxml2serializer.h"
#include <QIODevice>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include "regentry.h"
#include "regvar.h"
#include <QDebug>


#define RLXML_ABORT_ON_UNKNOWN_ELEMENT 0

#define RLXML_DEFAULT_BASE 10
#define RLXML_READ_BASE 0


static const QString element_regentries = QStringLiteral("RegEntries");
static const QString element_regentry = QStringLiteral("RegEntry");
static const QString element_regvars = QStringLiteral("RegVars");
static const QString element_regvar = QStringLiteral("RegVar");
// object.
static const QString element_name = QStringLiteral("name");
static const QString element_type = QStringLiteral("type");
static const QString element_descr = QStringLiteral("description");
// entry.
static const QString attr_index = QStringLiteral("index");
// var.
static const QString attr_subindex = QStringLiteral("subindex");
static const QString element_min = QStringLiteral("minimum");
static const QString element_max = QStringLiteral("maximum");
static const QString element_def = QStringLiteral("default");
static const QString element_flags = QStringLiteral("flags");
static const QString element_eflags = QStringLiteral("eflags");
static const QString element_base_index = QStringLiteral("base_index");
static const QString element_base_subindex = QStringLiteral("base_subindex");
static const QString element_count = QStringLiteral("count");
static const QString element_mem = QStringLiteral("memory");
//static const QString attr_ = QStringLiteral("");
//static const QString element_ = QStringLiteral("");



RegListXml2Serializer::RegListXml2Serializer(QObject *parent)
    : RegListSerializer(parent)
{
}

RegListXml2Serializer::~RegListXml2Serializer()
{
}


int RegListXml2Serializer::intValue(const QStringRef& str, int defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

int RegListXml2Serializer::intValue(const QString& str, int defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

unsigned int RegListXml2Serializer::uintValue(const QStringRef& str, unsigned int defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toUInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

unsigned int RegListXml2Serializer::uintValue(const QString& str, unsigned int defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toUInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

double RegListXml2Serializer::realValue(const QStringRef& str, double defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toDouble(&ok);
    if(ok) return val;
    return defVal;
}

double RegListXml2Serializer::realValue(const QString& str, double defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toDouble(&ok);
    if(ok) return val;
    return defVal;
}


bool RegListXml2Serializer::serialize(QIODevice* dev, const RegEntryList* reglist)
{
    if(dev == nullptr || !dev->isWritable()) return false;
    if(reglist == nullptr) return false;

    QXmlStreamWriter writer(dev);
    writer.setAutoFormatting(true);

    writer.writeStartDocument();

    if(!writeRegs(&writer, reglist)) return false;

    writer.writeEndDocument();

    return !writer.hasError();
}

bool RegListXml2Serializer::writeRegs(QXmlStreamWriter* writer, const RegEntryList* reglist) const
{
    writer->writeStartElement(element_regentries);
    for(auto& re: *reglist){
        if(!writeRegEntry(writer, re)) return false;
    }
    writer->writeEndElement();
    return !writer->hasError();
}

bool RegListXml2Serializer::writeRegEntry(QXmlStreamWriter* writer, const RegEntry* re) const
{
    writer->writeStartElement(element_regentry);
    writer->writeAttribute(attr_index, QString::number(static_cast<uint>(re->index()), RLXML_DEFAULT_BASE));
    writer->writeTextElement(element_name, re->name());
    writer->writeTextElement(element_type, QString::number(static_cast<uint>(re->type()), RLXML_DEFAULT_BASE));
    writer->writeTextElement(element_descr, re->description());

    if(!writeRegVars(writer, re)) return false;

    writer->writeEndElement();
    return !writer->hasError();
}

bool RegListXml2Serializer::writeRegVars(QXmlStreamWriter* writer, const RegEntry* re) const
{
    writer->writeStartElement(element_regvars);
    for(auto it = re->cbegin(); it != re->cend(); ++ it){
        if(!writeRegVar(writer, *it)) return false;
    }
    writer->writeEndElement();

    return !writer->hasError();
}

bool RegListXml2Serializer::writeRegVar(QXmlStreamWriter* writer, const RegVar* rv) const
{
    writer->writeStartElement(element_regvar);
    writer->writeAttribute(attr_subindex, QString::number(static_cast<uint>(rv->subIndex()), RLXML_DEFAULT_BASE));
    writer->writeTextElement(element_name, rv->name());
    writer->writeTextElement(element_type, QString::number(static_cast<uint>(rv->dataType()), RLXML_DEFAULT_BASE));
    writer->writeTextElement(element_count, QString::number(rv->count(), 10));
    writer->writeTextElement(element_mem, rv->memAddr());
    writer->writeTextElement(element_min, rv->minValue().toString());
    writer->writeTextElement(element_max, rv->maxValue().toString());
    writer->writeTextElement(element_def, rv->defaultValue().toString());
    writer->writeTextElement(element_base_index, QString::number(static_cast<uint>(rv->baseIndex()), RLXML_DEFAULT_BASE));
    writer->writeTextElement(element_base_subindex, QString::number(static_cast<uint>(rv->baseSubIndex()), RLXML_DEFAULT_BASE));
    writer->writeTextElement(element_flags, QString::number(static_cast<uint>(rv->flags()), RLXML_DEFAULT_BASE));
    writer->writeTextElement(element_eflags, QString::number(static_cast<uint>(rv->eflags()), RLXML_DEFAULT_BASE));
    writer->writeTextElement(element_descr, rv->description());
    writer->writeEndElement();
    return !writer->hasError();
}

bool RegListXml2Serializer::deserialize(QIODevice* dev, RegEntryList* reglist)
{
    if(dev == nullptr || !dev->isReadable()) return false;
    if(reglist == nullptr) return false;

    RegEntryList readed_regs;

    auto cleanup = [&readed_regs](bool ret_res) -> bool{
        qDeleteAll(readed_regs.begin(), readed_regs.end());
        return ret_res;
    };

    QXmlStreamReader reader(dev);

    while(!reader.atEnd()){
        QXmlStreamReader::TokenType token_type = reader.readNext();
        if(reader.hasError()) return cleanup(false);

        //qDebug() << "RegListXml2Serializer::deserialize" << token_type << reader.name();
        switch(token_type){
        case QXmlStreamReader::Invalid:
            return cleanup(false);
        case QXmlStreamReader::StartDocument:
            break;
        case QXmlStreamReader::EndDocument:
            break;
        case QXmlStreamReader::StartElement:
            if(reader.name() == element_regentries){
                if(!readRegs(&reader, &readed_regs)) return cleanup(false);
            }else{
                //unknown element.
                return cleanup(false);
            }
            break;
        case QXmlStreamReader::EndElement:
            break;
        default:
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            continue;
        }
    }

    if(reader.hasError()) return cleanup(false);

    reglist->append(readed_regs);

    return true;
}

bool RegListXml2Serializer::readRegs(QXmlStreamReader* reader, RegEntryList* reglist) const
{
    while(!reader->atEnd()){
        QXmlStreamReader::TokenType token_type = reader->readNext();
        if(reader->hasError()) return false;

        //qDebug() << "RegListXml2Serializer::readRegs" << token_type << reader->name();
        switch(token_type){
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            return false;
        case QXmlStreamReader::StartElement:
            if(reader->name() == element_regentry){
                RegEntry* re = new RegEntry();
                if(!readRegEntry(reader, re)){
                    delete re;
                    return false;
                }
                reglist->append(re);
            }else{
                // unknown element.
                return false;
            }
            break;
        case QXmlStreamReader::EndElement:
            return !reader->hasError();
        default:
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            continue;
        }
    }
    return false;
}

bool RegListXml2Serializer::readRegEntry(QXmlStreamReader* reader, RegEntry* re) const
{
    QXmlStreamAttributes attrs = reader->attributes();
    re->setIndex(static_cast<reg_index_t>(attrs.value(attr_index).toUInt(nullptr, 10)));

    while(!reader->atEnd()){
        QXmlStreamReader::TokenType token_type = reader->readNext();
        if(reader->hasError()) return false;

        //qDebug() << "RegListXml2Serializer::readRegEntry" << token_type << reader->name();
        switch(token_type){
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            return false;
        case QXmlStreamReader::StartElement:
            if(reader->name() == element_type){
                re->setType(static_cast<ObjectType>(reader->readElementText().toUInt(nullptr, RLXML_READ_BASE)));
            }
            else if(reader->name() == element_name){
                re->setName(reader->readElementText());
            }
            else if(reader->name() == element_descr){
                re->setDescription(reader->readElementText());
            }
            else if(reader->name() == element_regvars){
                if(!readRegVars(reader, re)) return false;
            }else{
                // unknown element.
#if defined(RLXML_ABORT_ON_UNKNOWN_ELEMENT) && RLXML_ABORT_ON_UNKNOWN_ELEMENT == 1
                return false;
#else
                reader->skipCurrentElement();
#endif
            }
            break;
        case QXmlStreamReader::EndElement:
            return !reader->hasError();
        default:
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            continue;
        }
    }

    return false;
}

bool RegListXml2Serializer::readRegVars(QXmlStreamReader* reader, RegEntry* re) const
{
    while(!reader->atEnd()){
        QXmlStreamReader::TokenType token_type = reader->readNext();
        if(reader->hasError()) return false;

        //qDebug() << "RegListXml2Serializer::readRegVars" << token_type << reader->name();
        switch(token_type){
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            return false;
        case QXmlStreamReader::StartElement:
            if(reader->name() == element_regvar){
                RegVar* rv = new RegVar();
                if(!readRegVar(reader, rv)){
                    delete rv;
                    return false;
                }
                re->add(rv);
            }else{
                // unknown element.
#if defined(RLXML_ABORT_ON_UNKNOWN_ELEMENT) && RLXML_ABORT_ON_UNKNOWN_ELEMENT == 1
                return false;
#else
                reader->skipCurrentElement();
#endif
            }
            break;
        case QXmlStreamReader::EndElement:
            return !reader->hasError();
        default:
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            continue;
        }
    }
    return false;
}

bool RegListXml2Serializer::readRegVar(QXmlStreamReader* reader, RegVar* rv) const
{
    QXmlStreamAttributes attrs = reader->attributes();
    rv->setSubIndex(static_cast<reg_subindex_t>(attrs.value(attr_subindex).toUInt(nullptr, RLXML_READ_BASE)));

    while(!reader->atEnd()){
        QXmlStreamReader::TokenType token_type = reader->readNext();
        if(reader->hasError()) return false;

        //qDebug() << "RegListXml2Serializer::readRegVar" << token_type << reader->name();
        switch(token_type){
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            return false;
        case QXmlStreamReader::StartElement:
            if(reader->name() == element_type){
                rv->setDataType(static_cast<DataType>(reader->readElementText().toUInt(nullptr, RLXML_READ_BASE)));
            }
            else if(reader->name() == element_name){
                rv->setName(reader->readElementText());
            }
            else if(reader->name() == element_descr){
                rv->setDescription(reader->readElementText());
            }
            else if(reader->name() == element_count){
                rv->setCount(reader->readElementText().toUInt(nullptr, RLXML_READ_BASE));
            }
            else if(reader->name() == element_mem){
                rv->setMemAddr(reader->readElementText());
            }
            else if(reader->name() == element_min){
                rv->setMinValue(reader->readElementText());
            }
            else if(reader->name() == element_max){
                rv->setMaxValue(reader->readElementText());
            }
            else if(reader->name() == element_def){
                rv->setDefaultValue(reader->readElementText());
            }
            else if(reader->name() == element_base_index){
                rv->setBaseIndex(static_cast<reg_index_t>(reader->readElementText().toUInt(nullptr, RLXML_READ_BASE)));
            }
            else if(reader->name() == element_base_subindex){
                rv->setBaseSubIndex(static_cast<reg_subindex_t>(reader->readElementText().toUInt(nullptr, RLXML_READ_BASE)));
            }
            else if(reader->name() == element_flags){
                rv->setFlags(static_cast<reg_flags_t>(reader->readElementText().toUInt(nullptr, RLXML_READ_BASE)));
            }
            else if(reader->name() == element_eflags){
                rv->setEFlags(static_cast<reg_eflags_t>(reader->readElementText().toUInt(nullptr, RLXML_READ_BASE)));
            }
            else{
                // unknown element.
#if defined(RLXML_ABORT_ON_UNKNOWN_ELEMENT) && RLXML_ABORT_ON_UNKNOWN_ELEMENT == 1
                return false;
#else
                reader->skipCurrentElement();
#endif
            }
            break;
        case QXmlStreamReader::EndElement:
            return !reader->hasError();
        default:
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            continue;
        }
    }
    return false;
}
