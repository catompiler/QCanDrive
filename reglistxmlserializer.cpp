#include "reglistxmlserializer.h"
#include <QIODevice>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include "regentry.h"
#include "regvar.h"
#include <QDebug>



static const QString element_regentries = QStringLiteral("RegEntries");
static const QString element_regentry = QStringLiteral("RegEntry");
static const QString element_regvars = QStringLiteral("RegVars");
static const QString element_regvar = QStringLiteral("RegVar");
// object.
static const QString attr_name = QStringLiteral("name");
static const QString attr_type = QStringLiteral("type");
static const QString attr_descr = QStringLiteral("description");
// entry.
static const QString attr_index = QStringLiteral("index");
// var.
static const QString attr_subindex = QStringLiteral("subindex");
static const QString attr_min = QStringLiteral("minimum");
static const QString attr_max = QStringLiteral("maximum");
static const QString attr_def = QStringLiteral("default");
static const QString attr_flags = QStringLiteral("flags");
static const QString attr_eflags = QStringLiteral("eflags");
static const QString attr_base_index = QStringLiteral("base_index");
static const QString attr_base_subindex = QStringLiteral("base_subindex");
static const QString attr_count = QStringLiteral("count");
static const QString attr_mem = QStringLiteral("memory");
//static const QString attr_ = QStringLiteral("");
static const QString element_ = QStringLiteral("");



RegListXmlSerializer::RegListXmlSerializer(QObject *parent)
    : RegListSerializer(parent)
{
}

RegListXmlSerializer::~RegListXmlSerializer()
{
}

bool RegListXmlSerializer::serialize(QIODevice* dev, const RegEntryList* reglist)
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

bool RegListXmlSerializer::writeRegs(QXmlStreamWriter* writer, const RegEntryList* reglist) const
{
    writer->writeStartElement(element_regentries);
    for(auto re: *reglist){
        if(!writeRegEntry(writer, re)) return false;
    }
    writer->writeEndElement();
    return !writer->hasError();
}

bool RegListXmlSerializer::writeRegEntry(QXmlStreamWriter* writer, const RegEntry* re) const
{
    writer->writeStartElement(element_regentry);
    writer->writeAttribute(attr_index, QString::number(static_cast<uint>(re->index()), 10));
    writer->writeAttribute(attr_name, re->name());
    writer->writeAttribute(attr_type, QString::number(static_cast<uint>(re->type()), 10));
    writer->writeAttribute(attr_descr, re->description());

    if(!writeRegVars(writer, re)) return false;

    writer->writeEndElement();
    return !writer->hasError();
}

bool RegListXmlSerializer::writeRegVars(QXmlStreamWriter* writer, const RegEntry* re) const
{
    writer->writeStartElement(element_regvars);
    for(auto it = re->cbegin(); it != re->cend(); ++ it){
        if(!writeRegVar(writer, *it)) return false;
    }
    writer->writeEndElement();

    return !writer->hasError();
}

bool RegListXmlSerializer::writeRegVar(QXmlStreamWriter* writer, const RegVar* rv) const
{
    writer->writeStartElement(element_regvar);
    writer->writeAttribute(attr_subindex, QString::number(static_cast<uint>(rv->subIndex()), 10));
    writer->writeAttribute(attr_name, rv->name());
    writer->writeAttribute(attr_type, QString::number(static_cast<uint>(rv->dataType()), 10));
    writer->writeAttribute(attr_count, QString::number(rv->count(), 10));
    writer->writeAttribute(attr_mem, rv->memAddr());
    writer->writeAttribute(attr_min, rv->minValue().toString());
    writer->writeAttribute(attr_max, rv->maxValue().toString());
    writer->writeAttribute(attr_def, rv->defaultValue().toString());
    writer->writeAttribute(attr_base_index, QString::number(static_cast<uint>(rv->baseIndex()), 10));
    writer->writeAttribute(attr_base_subindex, QString::number(static_cast<uint>(rv->baseSubIndex()), 10));
    writer->writeAttribute(attr_flags, QString::number(static_cast<uint>(rv->flags()), 10));
    writer->writeAttribute(attr_eflags, QString::number(static_cast<uint>(rv->eflags()), 10));
    writer->writeAttribute(attr_descr, rv->description());
    writer->writeEndElement();
    return !writer->hasError();
}

bool RegListXmlSerializer::deserialize(QIODevice* dev, RegEntryList* reglist)
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

        //qDebug() << "RegListXmlSerializer::deserialize" << token_type << reader.name();
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

bool RegListXmlSerializer::readRegs(QXmlStreamReader* reader, RegEntryList* reglist) const
{
    while(!reader->atEnd()){
        QXmlStreamReader::TokenType token_type = reader->readNext();
        if(reader->hasError()) return false;

        //qDebug() << "RegListXmlSerializer::readRegs" << token_type << reader->name();
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

bool RegListXmlSerializer::readRegEntry(QXmlStreamReader* reader, RegEntry* re) const
{
    QXmlStreamAttributes attrs = reader->attributes();
    re->setIndex(static_cast<reg_index_t>(attrs.value(attr_index).toUInt(nullptr, 10)));
    re->setType(static_cast<ObjectType>(attrs.value(attr_type).toUInt(nullptr, 10)));
    re->setName(attrs.value(attr_name).toString());
    re->setDescription(attrs.value(attr_descr).toString());

    while(!reader->atEnd()){
        QXmlStreamReader::TokenType token_type = reader->readNext();
        if(reader->hasError()) return false;

        //qDebug() << "RegListXmlSerializer::readRegEntry" << token_type << reader->name();
        switch(token_type){
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            return false;
        case QXmlStreamReader::StartElement:
            if(reader->name() == element_regvars){
                if(!readRegVars(reader, re)) return false;
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

bool RegListXmlSerializer::readRegVars(QXmlStreamReader* reader, RegEntry* re) const
{
    while(!reader->atEnd()){
        QXmlStreamReader::TokenType token_type = reader->readNext();
        if(reader->hasError()) return false;

        //qDebug() << "RegListXmlSerializer::readRegVars" << token_type << reader->name();
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

bool RegListXmlSerializer::readRegVar(QXmlStreamReader* reader, RegVar* rv) const
{
    QXmlStreamAttributes attrs = reader->attributes();
    rv->setSubIndex(static_cast<reg_subindex_t>(attrs.value(attr_subindex).toUInt(nullptr, 10)));
    rv->setName(attrs.value(attr_name).toString());
    rv->setDataType(static_cast<DataType>(attrs.value(attr_type).toUInt(nullptr, 10)));
    rv->setCount(attrs.value(attr_count).toUInt(nullptr, 10));
    rv->setMemAddr(attrs.value(attr_mem).toString());
    rv->setMinValue(attrs.value(attr_min).toString());
    rv->setMaxValue(attrs.value(attr_max).toString());
    rv->setDefaultValue(attrs.value(attr_def).toString());
    rv->setBaseIndex(static_cast<reg_index_t>(attrs.value(attr_base_index).toUInt(nullptr, 10)));
    rv->setBaseSubIndex(static_cast<reg_subindex_t>(attrs.value(attr_base_subindex).toUInt(nullptr, 10)));
    rv->setFlags(static_cast<reg_flags_t>(attrs.value(attr_flags).toUInt(nullptr, 10)));
    rv->setEFlags(static_cast<reg_eflags_t>(attrs.value(attr_eflags).toUInt(nullptr, 10)));
    rv->setDescription(attrs.value(attr_descr).toString());

    while(!reader->atEnd()){
        QXmlStreamReader::TokenType token_type = reader->readNext();
        if(reader->hasError()) return false;

        //qDebug() << "RegListXmlSerializer::readRegVar" << token_type << reader->name();
        switch(token_type){
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            return false;
        case QXmlStreamReader::StartElement:
            // unknown element.
            return false;
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
