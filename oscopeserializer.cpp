#include "oscopeserializer.h"
#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "stringutils.h"
#include "oscopeplot.h"
#include <QDebug>


using namespace StringUtils;


#define OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT 0



OScopeSerializer::OScopeSerializer()
{
}

OScopeSerializer::~OScopeSerializer()
{
}

bool OScopeSerializer::serialize(QIODevice* dev, const ScopeConf& conf) const
{
    if(!dev->isWritable()) return false;

    QXmlStreamWriter xml(dev);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(4);

    xml.writeStartDocument();

    bool res = writeOScope(xml, conf);

    xml.writeEndDocument();

    if(!res){
        //cleanup
        return false;
    }

    return true;
}

bool OScopeSerializer::deserialize(QIODevice* dev, ScopeConf& conf) const
{
    if(!dev->isReadable()) return false;

    QXmlStreamReader xml(dev);

    while(!xml.isStartElement() && !xml.atEnd()){
        xml.readNextStartElement();
    }

    bool res = readOScope(xml, conf);

    //cleanup

    return res;
}

bool OScopeSerializer::writeOScope(QXmlStreamWriter& xml, const ScopeConf& conf) const
{
    xml.writeStartElement("oscope");

    if(!writeCommon(xml, conf.common)) return false;
    if(!writeTrigger(xml, conf.trigger)) return false;
    if(!writeCursors(xml, conf.cursors)) return false;
    if(!writeChannels(xml, conf.channels)) return false;

    xml.writeEndElement();

    return true;
}

bool OScopeSerializer::readOScope(QXmlStreamReader& xml, ScopeConf& conf) const
{
    if(!xml.isStartElement() || xml.name() != "oscope") return false;

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        switch(token){
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            res = false;
            done = true;
            break;
        case QXmlStreamReader::StartElement:{
            if(readCommon(xml, conf.common)){
                break;
            }
            else if(readTrigger(xml, conf.trigger)){
                break;
            }
            else if(readCursors(xml, conf.cursors)){
                break;
            }
            else if(readChannels(xml, conf.channels)){
                break;
            }
            else{
#if defined(OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT) && OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT == 1
                res = false;
                done = true;
#else
                xml.skipCurrentElement();
#endif
            }
        }break;
        case QXmlStreamReader::EndElement:
            done = true;
            break;
        case QXmlStreamReader::Characters:
            if(!xml.isWhitespace()){
                res = false;
                done = true;
            }
            break;
        case QXmlStreamReader::Comment:
            break;
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            break;
        }
    }

    if(res == false){
        //cleanup
    }

    return res;
}

bool OScopeSerializer::writeCommon(QXmlStreamWriter& xml, const CommonConf& com_conf) const
{
    xml.writeStartElement("common");

    xml.writeTextElement("prescaler", QString::number(com_conf.prescaler));
    xml.writeTextElement("samplesCount", QString::number(com_conf.samplesCount));
    xml.writeTextElement("histSamplesCount", QString::number(com_conf.histSamplesCount));
    xml.writeTextElement("hDiv", QString::number(com_conf.hDiv));
    xml.writeTextElement("hOffset", QString::number(com_conf.hOffset));

    xml.writeEndElement();

    return true;
}

bool OScopeSerializer::readCommon(QXmlStreamReader& xml, CommonConf& com_conf) const
{
    if(!xml.isStartElement()) return false;
    if(xml.name() != "common") return false;

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        switch(token){
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            res = false;
            done = true;
            break;
        case QXmlStreamReader::StartElement:{
            auto name = xml.name();
            if(name == "prescaler"){
                com_conf.prescaler = uintValue(xml.readElementText(), 0);
            }
            else if(name == "samplesCount"){
                com_conf.samplesCount = uintValue(xml.readElementText(), 128);
            }
            else if(name == "histSamplesCount"){
                com_conf.histSamplesCount = uintValue(xml.readElementText(), 64);
            }
            else if(name == "hDiv"){
                com_conf.hDiv = realValue(xml.readElementText(), 1.0);
            }
            else if(name == "hOffset"){
                com_conf.hOffset = realValue(xml.readElementText(), 0.0);
            }
            else{
#if defined(OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT) && OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT == 1
                res = false;
                done = true;
#else
                xml.skipCurrentElement();
#endif
            }
        }break;
        case QXmlStreamReader::EndElement:
            done = true;
            break;
        case QXmlStreamReader::Characters:
            if(!xml.isWhitespace()){
                res = false;
                done = true;
            }
            break;
        case QXmlStreamReader::Comment:
            break;
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            break;
        }
    }

    if(res == false){
        //cleanup
    }

    return res;
}

bool OScopeSerializer::writeTrigger(QXmlStreamWriter& xml, const TriggerConf& trig_conf) const
{
    xml.writeStartElement("trigger");

    xml.writeTextElement("enabled", QString::number(trig_conf.enabled));
    xml.writeTextElement("channel", QString::number(trig_conf.channel));
    xml.writeTextElement("type", QString::number(static_cast<uint>(trig_conf.type)));
    xml.writeTextElement("value", QString::number(trig_conf.value));

    xml.writeEndElement();

    return true;
}

bool OScopeSerializer::readTrigger(QXmlStreamReader& xml, TriggerConf& trig_conf) const
{
    if(!xml.isStartElement()) return false;
    if(xml.name() != "trigger") return false;

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        switch(token){
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            res = false;
            done = true;
            break;
        case QXmlStreamReader::StartElement:{
            auto name = xml.name();
            if(name == "enabled"){
                trig_conf.enabled = uintValue(xml.readElementText(), 0);
            }
            else if(name == "channel"){
                trig_conf.channel = uintValue(xml.readElementText(), 0);
            }
            else if(name == "type"){
                trig_conf.type = static_cast<SDOScope::TriggerType>(uintValue(xml.readElementText(), static_cast<uint>(SDOScope::TRIGGER_RISING)));
            }
            else if(name == "value"){
                trig_conf.value = uintValue(xml.readElementText(), 0);
            }
            else{
#if defined(OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT) && OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT == 1
                res = false;
                done = true;
#else
                xml.skipCurrentElement();
#endif
            }
        }break;
        case QXmlStreamReader::EndElement:
            done = true;
            break;
        case QXmlStreamReader::Characters:
            if(!xml.isWhitespace()){
                res = false;
                done = true;
            }
            break;
        case QXmlStreamReader::Comment:
            break;
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            break;
        }
    }

    if(res == false){
        //cleanup
    }

    return res;
}

bool OScopeSerializer::writeCursors(QXmlStreamWriter& xml, const CursorsConf& cur_conf) const
{
    xml.writeStartElement("cursors");

    if(!writeFloatingCursor(xml, cur_conf.floating)) return false;

    xml.writeEndElement();

    return true;
}

bool OScopeSerializer::readCursors(QXmlStreamReader& xml, CursorsConf& cur_conf) const
{
    if(!xml.isStartElement() || xml.name() != "cursors") return false;

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        switch(token){
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            res = false;
            done = true;
            break;
        case QXmlStreamReader::StartElement:{
            if(readFloatingCursor(xml, cur_conf.floating)){
                break;
            }
            else{
#if defined(OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT) && OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT == 1
                res = false;
                done = true;
#else
                xml.skipCurrentElement();
#endif
            }
        }break;
        case QXmlStreamReader::EndElement:
            done = true;
            break;
        case QXmlStreamReader::Characters:
            if(!xml.isWhitespace()){
                res = false;
                done = true;
            }
            break;
        case QXmlStreamReader::Comment:
            break;
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            break;
        }
    }

    if(res == false){
        //cleanup
    }

    return res;
}

bool OScopeSerializer::writeFloatingCursor(QXmlStreamWriter& xml, const FloatingCursorConf& fl_conf) const
{
    xml.writeStartElement("floating");

    xml.writeTextElement("enabled", QString::number(fl_conf.enabled));

    xml.writeEndElement();

    return true;
}

bool OScopeSerializer::readFloatingCursor(QXmlStreamReader& xml, FloatingCursorConf& fl_conf) const
{
    if(!xml.isStartElement()) return false;
    if(xml.name() != "floating") return false;

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        switch(token){
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            res = false;
            done = true;
            break;
        case QXmlStreamReader::StartElement:{
            auto name = xml.name();
            if(name == "enabled"){
                fl_conf.enabled = uintValue(xml.readElementText(), 0);
            }
            else{
#if defined(OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT) && OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT == 1
                res = false;
                done = true;
#else
                xml.skipCurrentElement();
#endif
            }
        }break;
        case QXmlStreamReader::EndElement:
            done = true;
            break;
        case QXmlStreamReader::Characters:
            if(!xml.isWhitespace()){
                res = false;
                done = true;
            }
            break;
        case QXmlStreamReader::Comment:
            break;
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            break;
        }
    }

    if(res == false){
        //cleanup
    }

    return res;
}

bool OScopeSerializer::writeChannels(QXmlStreamWriter& xml, const ChannelsConf& chs_conf) const
{
    xml.writeStartElement("channels");

    for(auto& ch_conf: chs_conf){
        if(!writeChannel(xml, ch_conf)) return false;
    }

    xml.writeEndElement();

    return true;
}

bool OScopeSerializer::readChannels(QXmlStreamReader& xml, ChannelsConf& chs_conf) const
{
    if(!xml.isStartElement() || xml.name() != "channels") return false;

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        switch(token){
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            res = false;
            done = true;
            break;
        case QXmlStreamReader::StartElement:{
            if(readChannel(xml, chs_conf)){
                break;
            }
            else{
#if defined(OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT) && OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT == 1
                res = false;
                done = true;
#else
                xml.skipCurrentElement();
#endif
            }
        }break;
        case QXmlStreamReader::EndElement:
            done = true;
            break;
        case QXmlStreamReader::Characters:
            if(!xml.isWhitespace()){
                res = false;
                done = true;
            }
            break;
        case QXmlStreamReader::Comment:
            break;
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            break;
        }
    }

    if(res == false){
        //cleanup
    }

    return res;
}

bool OScopeSerializer::writeChannel(QXmlStreamWriter& xml, const ChannelConf& ch_conf) const
{
    xml.writeStartElement("channel");

    xml.writeTextElement("enabled", QString::number(ch_conf.enabled));
    xml.writeTextElement("index", QString::number(ch_conf.index));
    xml.writeTextElement("subIndex", QString::number(ch_conf.subIndex));
    xml.writeTextElement("dataType", QString::number(static_cast<uint>(ch_conf.dataType)));
    xml.writeTextElement("baseValue", QString::number(ch_conf.baseValue));
    xml.writeTextElement("vDiv", QString::number(ch_conf.vDiv));
    xml.writeTextElement("vOffset", QString::number(ch_conf.vOffset));
    xml.writeTextElement("visible", QString::number(ch_conf.visible));
    xml.writeTextElement("name", ch_conf.name);
    xml.writeTextElement("penColor", QString::number(ch_conf.penColor.rgba()));
    xml.writeTextElement("penStyle", QString::number(ch_conf.penStyle));
    xml.writeTextElement("penWidthF", QString::number(ch_conf.penWidthF));

    xml.writeEndElement();

    return true;
}

bool OScopeSerializer::readChannel(QXmlStreamReader& xml, ChannelsConf& chs_conf) const
{
    if(!xml.isStartElement()) return false;
    if(xml.name() != "channel") return false;

    ChannelConf ch_conf;

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        switch(token){
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
            res = false;
            done = true;
            break;
        case QXmlStreamReader::StartElement:{
            auto name = xml.name();
            if(name == "enabled"){
                ch_conf.enabled = uintValue(xml.readElementText(), 0);
            }
            else if(name == "index"){
                ch_conf.index = uintValue(xml.readElementText(), 0);
            }
            else if(name == "subIndex"){
                ch_conf.subIndex = uintValue(xml.readElementText(), 0);
            }
            else if(name == "dataType"){
                ch_conf.dataType = static_cast<DataType>(uintValue(xml.readElementText(), static_cast<uint>(DataType::IQ24)));
            }
            else if(name == "baseValue"){
                ch_conf.baseValue = realValue(xml.readElementText(), 0.0);
            }
            else if(name == "vDiv"){
                ch_conf.vDiv = realValue(xml.readElementText(), 1.0);
            }
            else if(name == "vOffset"){
                ch_conf.vOffset = realValue(xml.readElementText(), 0);
            }
            else if(name == "visible"){
                ch_conf.visible = uintValue(xml.readElementText(), 0);
            }
            else if(name == "name"){
                ch_conf.name = xml.readElementText();
            }
            else if(name == "penColor"){
                ch_conf.penColor = QColor::fromRgba(uintValue(xml.readElementText(), OScopePlot::getDefaultColor(chs_conf.count())));
            }
            else if(name == "penStyle"){
                ch_conf.penStyle = static_cast<Qt::PenStyle>(uintValue(xml.readElementText(), static_cast<uint>(OScopePlot::defaultLineStyle)));
            }
            else if(name == "penWidthF"){
                ch_conf.penWidthF = realValue(xml.readElementText(), OScopePlot::defaultLineWidthF);
            }
            else{
#if defined(OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT) && OSCP_SER_ABORT_ON_UNKNOWN_ELEMENT == 1
                res = false;
                done = true;
#else
                xml.skipCurrentElement();
#endif
            }
        }break;
        case QXmlStreamReader::EndElement:
            done = true;
            break;
        case QXmlStreamReader::Characters:
            if(!xml.isWhitespace()){
                res = false;
                done = true;
            }
            break;
        case QXmlStreamReader::Comment:
            break;
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            break;
        }
    }

    if(res == false){
        //cleanup
    }else{
        chs_conf.append(ch_conf);
    }

    return res;
}
