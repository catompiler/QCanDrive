#include "cockpitserializer.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QLayout>
#include <QString>
#include "sdovalue.h"
#include "sdovalueplot.h"
#include "sdovaluedial.h"
#include "sdovalueslider.h"
#include "sdovaluebar.h"
#include "sdovaluebutton.h"
#include "sdovalueindicator.h"
#include <QDebug>


#define CS_ABORT_ON_UNKNOWN_ELEMENT 0



CockpitSerializer::CockpitSerializer()
{
    m_valsHolder = nullptr;
}

CockpitSerializer::~CockpitSerializer()
{

}

CoValuesHolder* CockpitSerializer::sdoValuesHolder() const
{
    return m_valsHolder;
}

void CockpitSerializer::setSdoValuesHolder(CoValuesHolder* newSdoValuesHolder)
{
    m_valsHolder = newSdoValuesHolder;
}

bool CockpitSerializer::serialize(QIODevice* dev, const CockpitWidgets& widgets) const
{
    if(!dev->isWritable()) return false;

    QXmlStreamWriter xml(dev);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(4);

    xml.writeStartDocument();
    xml.writeStartElement("cockpit");

    bool res = false;

    res = writeCockpit(xml, widgets);

    xml.writeEndElement();
    xml.writeEndDocument();

    if(!res){
        //cleanup
        return false;
    }

    return true;
}

CockpitSerializer::CockpitWidgets CockpitSerializer::deserialize(QIODevice* dev, bool* isOk) const
{
    CockpitWidgets widgets;

    if(!dev->isReadable()){
        if(isOk) *isOk = false;
        return widgets;
    }

    QXmlStreamReader xml(dev);

    while(!xml.isStartElement() && !xml.atEnd()){
        xml.readNextStartElement();
    }

    widgets = readCockpit(xml, isOk);

    return widgets;
}

int CockpitSerializer::intValue(const QStringRef& str, int defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

int CockpitSerializer::intValue(const QString& str, int defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

unsigned int CockpitSerializer::uintValue(const QStringRef& str, unsigned int defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toUInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

unsigned int CockpitSerializer::uintValue(const QString& str, unsigned int defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toUInt(&ok, 0);
    if(ok) return val;
    return defVal;
}

double CockpitSerializer::realValue(const QStringRef& str, double defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toDouble(&ok);
    if(ok) return val;
    return defVal;
}

double CockpitSerializer::realValue(const QString& str, double defVal) const
{
    bool ok = false;
    decltype(defVal) val = str.toDouble(&ok);
    if(ok) return val;
    return defVal;
}

CockpitSerializer::CockpitWidgets CockpitSerializer::readCockpit(QXmlStreamReader& xml, bool* isOk) const
{
    CockpitWidgets widgets;

    if(!xml.isStartElement() || xml.name() != "cockpit"){
        if(isOk) *isOk = false;
        return widgets;
    }

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        //qDebug() << "token:" << token << xml.name();

        switch(token){
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Invalid:
            //qDebug() << "error:" << xml.error();
            res = false;
            done = true;
            break;
        case QXmlStreamReader::StartDocument:
            break;
        case QXmlStreamReader::EndDocument:
            done = true;
            break;
        case QXmlStreamReader::StartElement:{
                QRect r = readPosAttribs(xml);

                if(auto rdw = readSDOValuePlot(xml)){
                    widgets.append(qMakePair(rdw, r));
                    break;
                }

                else if(auto rdw = readSDOValueDial(xml)){
                    widgets.append(qMakePair(rdw, r));
                    break;
                }

                else if(auto rdw = readSDOValueSlider(xml)){
                    widgets.append(qMakePair(rdw, r));
                    break;
                }

                else if(auto rdw = readSDOValueBar(xml)){
                    widgets.append(qMakePair(rdw, r));
                    break;
                }

                else if(auto rdw = readSDOValueButton(xml)){
                    widgets.append(qMakePair(rdw, r));
                    break;
                }

                else if(auto rdw = readSDOValueIndicator(xml)){
                    widgets.append(qMakePair(rdw, r));
                    break;
                }

#if defined(CS_ABORT_ON_UNKNOWN_ELEMENT) && CS_ABORT_ON_UNKNOWN_ELEMENT == 1
                res = false;
                done = true;
#else
                xml.skipCurrentElement();
#endif
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

    if(!res){
        //cleanup
        for(auto& p: widgets){
            delete p.first;
        }
        widgets.clear();
    }

    if(isOk) *isOk = res;
    return widgets;
}

bool CockpitSerializer::writeCockpit(QXmlStreamWriter& xml, const CockpitWidgets& widgets) const
{
    bool res = true;

    for(const auto& p: widgets){
        QWidget* w = p.first;
        const QRect& pos = p.second;

        if(w == nullptr) continue;

        if(SDOValuePlot* wgt = qobject_cast<SDOValuePlot*>(w)){
            res = writeSDOValuePlot(xml, wgt, pos);
            if(res == false) break;
        }


        else if(SDOValueDial* wgt = qobject_cast<SDOValueDial*>(w)){
            res = writeSDOValueDial(xml, wgt, pos);
            if(res == false) break;
        }


        else if(SDOValueSlider* wgt = qobject_cast<SDOValueSlider*>(w)){
            res = writeSDOValueSlider(xml, wgt, pos);
            if(res == false) break;
        }


        else if(SDOValueBar* wgt = qobject_cast<SDOValueBar*>(w)){
            res = writeSDOValueBar(xml, wgt, pos);
            if(res == false) break;
        }


        else if(SDOValueButton* wgt = qobject_cast<SDOValueButton*>(w)){
            res = writeSDOValueButton(xml, wgt, pos);
            if(res == false) break;
        }


        else if(SDOValueIndicator* wgt = qobject_cast<SDOValueIndicator*>(w)){
            res = writeSDOValueIndicator(xml, wgt, pos);
            if(res == false) break;
        }
    }

    return res;
}

QRect CockpitSerializer::readPosAttribs(QXmlStreamReader& xml) const
{
    auto attrs = xml.attributes();
    int x = intValue(attrs.value("row"), 0);
    int y = intValue(attrs.value("col"), 0);
    int w = intValue(attrs.value("rowSpan"), 1);
    int h = intValue(attrs.value("colSpan"), 1);

    return QRect(x, y, w, h);
}

void CockpitSerializer::writePosAttribs(QXmlStreamWriter& xml, const QRect& pos) const
{
    xml.writeAttribute("row", QString::number(pos.x()));
    xml.writeAttribute("col", QString::number(pos.y()));
    xml.writeAttribute("rowSpan", QString::number(pos.width()));
    xml.writeAttribute("colSpan", QString::number(pos.height()));
}

std::tuple<CO::NodeId, CO::Index, CO::SubIndex, COValue::Type> CockpitSerializer::readCoAttribs(QXmlStreamReader& xml) const
{
    auto attrs = xml.attributes();
    CO::NodeId nodeId = intValue(attrs.value("nodeId"), 0);
    CO::Index index = intValue(attrs.value("index"), 0);
    CO::SubIndex subIndex = intValue(attrs.value("subIndex"), 0);
    COValue::Type type = static_cast<COValue::Type>(intValue(attrs.value("type"), 0));

    return std::make_tuple(nodeId, index, subIndex, type);
}

void CockpitSerializer::writeCoAttribs(QXmlStreamWriter& xml, CO::NodeId nid, CO::Index idx, CO::SubIndex sidx, COValue::Type valtype) const
{
    xml.writeAttribute("nodeId", QString::number(nid));
    xml.writeAttribute("index", QString::number(idx));
    xml.writeAttribute("subIndex", QString::number(sidx));
    xml.writeAttribute("type", QString::number(valtype));
}

bool CockpitSerializer::writeSDOValuePlot(QXmlStreamWriter& xml, const SDOValuePlot* plt, const QRect& pos) const
{
    xml.writeStartElement("SDOValuePlot");
    writePosAttribs(xml, pos);

    xml.writeTextElement("name", plt->name());
    xml.writeTextElement("bufferSize", QString::number(plt->bufferSize()));
    xml.writeTextElement("defaultAlpha", QString::number(plt->defaultAlpha()));
    xml.writeTextElement("backgroundColor", QString::number(plt->background().color().rgb()));
    xml.writeTextElement("backgroundStyle", QString::number(plt->background().style()));
    xml.writeTextElement("textColor", QString::number(plt->textColor().rgb()));
    xml.writeTextElement("signalsCount", QString::number(plt->signalsCount()));
    //xml.writeTextElement("period", QString::number(plt->period()));
    //xml.writeTextElement("", QString::number(plt->));

    for(int i = 0; i < plt->signalsCount(); i ++){
        xml.writeStartElement("signalCurve");

        auto sdoval = plt->getSDOValue(i);
        if(sdoval != nullptr){
            writeCoAttribs(xml, sdoval->nodeId(), sdoval->index(),
                           sdoval->subIndex(), plt->SDOValueType(i));
        }

        xml.writeTextElement("name", plt->signalName(i));
        xml.writeTextElement("z", QString::number(plt->z(i)));
        xml.writeTextElement("curveStyle", QString::number(plt->curveStyle(i)));
        xml.writeTextElement("curvePenColor", QString::number(plt->pen(i).color().rgba()));
        xml.writeTextElement("curvePenStyle", QString::number(plt->pen(i).style()));
        xml.writeTextElement("curveBrushColor", QString::number(plt->brush(i).color().rgba()));
        xml.writeTextElement("curveBrushStyle", QString::number(plt->brush(i).style()));
        xml.writeTextElement("baseLine", QString::number(plt->baseLine(i)));
        xml.writeEndElement();
    }

    xml.writeEndElement();

    return true;
}

SDOValuePlot* CockpitSerializer::readSDOValuePlot(QXmlStreamReader& xml) const
{
    //qDebug() << "readSDOValuePlot";

    if(!xml.isStartElement()) return nullptr;
    if(xml.name() != "SDOValuePlot") return nullptr;

    SDOValuePlot* plt = new SDOValuePlot(QString(), m_valsHolder);

    int countOfSignals = 0;

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
            if(name == "name"){
                plt->setName(xml.readElementText());
            }
            else if(name == "bufferSize"){
                plt->setBufferSize(uintValue(xml.readElementText(), 0));
            }
            else if(name == "defaultAlpha"){
                plt->setDefaultAlpha(realValue(xml.readElementText(), 0));
            }
            else if(name == "backgroundColor"){
                QBrush b = plt->background();
                b.setColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
                plt->setBackground(b);
            }
            else if(name == "backgroundStyle"){
                QBrush b = plt->background();
                b.setStyle(static_cast<Qt::BrushStyle>(uintValue(xml.readElementText(), 0)));
                plt->setBackground(b);
            }
            else if(name == "textColor"){
                plt->setTextColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "signalsCount"){
                countOfSignals = intValue(xml.readElementText(), 0);
            }
            /*else if(name == "period"){
                plt->setPeriod(realValue(xml.readElementText(), 0));
            }*/
            else if(name == "signalCurve"){
                auto co = readCoAttribs(xml);

                bool added = plt->addSDOValue(std::get<0>(co), std::get<1>(co),
                                              std::get<2>(co), std::get<3>(co));
                if(!added){
                    res = false;
                    done = true;
                    break;
                }
                if(!readSDOValuePlotSignalCurve(xml, plt, plt->SDOValuesCount() - 1)){
                    res = false;
                    done = true;
                    break;
                }
            }
            else{
#if defined(CS_ABORT_ON_UNKNOWN_ELEMENT) && CS_ABORT_ON_UNKNOWN_ELEMENT == 1
                res = false;
                done = true;
#else
                xml.skipCurrentElement();
#endif
            }
        }break;
        case QXmlStreamReader::EndElement:
            if(plt->SDOValuesCount() != countOfSignals){
                res = false;
            }
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
        delete plt;
        plt = nullptr;
    }

    return plt;
}

bool CockpitSerializer::readSDOValuePlotSignalCurve(QXmlStreamReader& xml, SDOValuePlot* plt, int signum) const
{
    if(!xml.isStartElement()) return false;
    if(xml.name() != "signalCurve") return false;

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
            if(name == "name"){
                plt->setSignalName(signum, xml.readElementText());
            }
            else if(name == "z"){
                plt->setZ(signum, realValue(xml.readElementText(), 0));
            }
            else if(name == "curveStyle"){
                auto cs = static_cast<QwtPlotCurve::CurveStyle>(uintValue(xml.readElementText(), 0));
                plt->setCurveStyle(signum, cs);
            }
            else if(name == "curvePenColor"){
                QPen p = plt->pen(signum);
                p.setColor(QColor::fromRgba(uintValue(xml.readElementText(), 0)));
                plt->setPen(signum, p);
            }
            else if(name == "curvePenStyle"){
                QPen p = plt->pen(signum);
                p.setStyle(static_cast<Qt::PenStyle>(uintValue(xml.readElementText(), 0)));
                plt->setPen(signum, p);
            }
            else if(name == "curveBrushColor"){
                QBrush b = plt->brush(signum);
                b.setColor(QColor::fromRgba(uintValue(xml.readElementText(), 0)));
                plt->setBrush(signum, b);
            }
            else if(name == "curveBrushStyle"){
                QBrush b = plt->brush(signum);
                b.setStyle(static_cast<Qt::BrushStyle>(uintValue(xml.readElementText(), 0)));
                plt->setBrush(signum, b);
            }
            else if(name == "baseLine"){
                plt->setBaseLine(signum, realValue(xml.readElementText(), 0));
            }
            else{
#if defined(CS_ABORT_ON_UNKNOWN_ELEMENT) && CS_ABORT_ON_UNKNOWN_ELEMENT == 1
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

    return res;
}


bool CockpitSerializer::writeSDOValueDial(QXmlStreamWriter& xml, const SDOValueDial* dl, const QRect& pos) const
{
    xml.writeStartElement("SDOValueDial");
    writePosAttribs(xml, pos);
    if(auto sdoval = dl->getSDOValue()){
        writeCoAttribs(xml, sdoval->nodeId(), sdoval->index(),
                       sdoval->subIndex(), dl->SDOValueType());
    }

    xml.writeTextElement("name", dl->name());
    xml.writeTextElement("outsideBackColor", QString::number(dl->outsideBackColor().rgb()));
    xml.writeTextElement("insideBackColor", QString::number(dl->insideBackColor().rgb()));
    xml.writeTextElement("insideScaleBackColor", QString::number(dl->insideScaleBackColor().rgb()));
    xml.writeTextElement("textScaleColor", QString::number(dl->textScaleColor().rgb()));
    xml.writeTextElement("needleColor", QString::number(dl->needleColor().rgb()));
    xml.writeTextElement("penWidth", QString::number(dl->penWidth()));
    xml.writeTextElement("precision", QString::number(dl->precision()));
    xml.writeTextElement("rangeMin", QString::number(dl->rangeMin()));
    xml.writeTextElement("rangeMax", QString::number(dl->rangeMax()));

    xml.writeEndElement();

    return true;
}

SDOValueDial* CockpitSerializer::readSDOValueDial(QXmlStreamReader& xml) const
{
    //qDebug() << "readSDOValueDial";

    if(!xml.isStartElement()) return nullptr;
    if(xml.name() != "SDOValueDial") return nullptr;

    SDOValueDial* dl = new SDOValueDial(m_valsHolder);

    auto co = readCoAttribs(xml);

    if(!dl->setSDOValue(std::get<0>(co), std::get<1>(co), std::get<2>(co), std::get<3>(co))){
        delete dl;
        return nullptr;
    }

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
            if(name == "name"){
                dl->setName(xml.readElementText());
            }
            else if(name == "outsideBackColor"){
                dl->setOutsideBackColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "insideBackColor"){
                dl->setInsideBackColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "insideScaleBackColor"){
                dl->setInsideScaleBackColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "textScaleColor"){
                dl->setTextScaleColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "needleColor"){
                dl->setNeedleColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "penWidth"){
                dl->setPenWidth(realValue(xml.readElementText(), 0));
            }
            else if(name == "precision"){
                dl->setPrecision(uintValue(xml.readElementText(), 0));
            }
            else if(name == "rangeMin"){
                dl->setRangeMin(realValue(xml.readElementText(), 0));
            }
            else if(name == "rangeMax"){
                dl->setRangeMax(realValue(xml.readElementText(), 0));
            }
            else{
#if defined(CS_ABORT_ON_UNKNOWN_ELEMENT) && CS_ABORT_ON_UNKNOWN_ELEMENT == 1
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
        delete dl;
        dl = nullptr;
    }

    return dl;
}


bool CockpitSerializer::writeSDOValueSlider(QXmlStreamWriter& xml, const SDOValueSlider* sl, const QRect& pos) const
{
    xml.writeStartElement("SDOValueSlider");
    writePosAttribs(xml, pos);
    if(auto sdoval = sl->getSDOValue()){
        writeCoAttribs(xml, sdoval->nodeId(), sdoval->index(),
                       sdoval->subIndex(), sl->SDOValueType());
    }

    xml.writeTextElement("name", sl->name());
    xml.writeTextElement("troughColor", QString::number(sl->troughColor().rgb()));
    xml.writeTextElement("grooveColor", QString::number(sl->grooveColor().rgb()));
    xml.writeTextElement("handleColor", QString::number(sl->handleColor().rgb()));
    xml.writeTextElement("scaleColor", QString::number(sl->scaleColor().rgb()));
    xml.writeTextElement("textColor", QString::number(sl->textColor().rgb()));
    xml.writeTextElement("penWidth", QString::number(sl->penWidth()));
    xml.writeTextElement("steps", QString::number(sl->steps()));
    xml.writeTextElement("hasTrough", QString::number(sl->hasTrough()));
    xml.writeTextElement("hasGroove", QString::number(sl->hasGroove()));
    xml.writeTextElement("orientation", QString::number(sl->orientation()));
    xml.writeTextElement("rangeMin", QString::number(sl->rangeMin()));
    xml.writeTextElement("rangeMax", QString::number(sl->rangeMax()));

    xml.writeEndElement();

    return true;
}

SDOValueSlider* CockpitSerializer::readSDOValueSlider(QXmlStreamReader& xml) const
{
    //qDebug() << "readSDOValueSlider";

    if(!xml.isStartElement()) return nullptr;
    if(xml.name() != "SDOValueSlider") return nullptr;

    SDOValueSlider* sl = new SDOValueSlider(m_valsHolder);

    auto co = readCoAttribs(xml);

    if(!sl->setSDOValue(std::get<0>(co), std::get<1>(co), std::get<2>(co), std::get<3>(co))){
        delete sl;
        return nullptr;
    }

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        //qDebug() << "token:" << token << xml.name();

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
            if(name == "name"){
                sl->setName(xml.readElementText());
            }
            else if(name == "troughColor"){
                sl->setTroughColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "grooveColor"){
                sl->setGrooveColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "handleColor"){
                sl->setHandleColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "scaleColor"){
                sl->setScaleColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "textColor"){
                sl->setTextColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "penWidth"){
                sl->setPenWidth(realValue(xml.readElementText(), 0));
            }
            else if(name == "steps"){
                sl->setSteps(uintValue(xml.readElementText(), 0));
            }
            else if(name == "hasTrough"){
                sl->setHasTrough(uintValue(xml.readElementText(), 0));
            }
            else if(name == "hasGroove"){
                sl->setHasGroove(uintValue(xml.readElementText(), 0));
            }
            else if(name == "orientation"){
                sl->setOrientation(static_cast<Qt::Orientation>(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "rangeMin"){
                sl->setRangeMin(realValue(xml.readElementText(), 0));
            }
            else if(name == "rangeMax"){
                sl->setRangeMax(realValue(xml.readElementText(), 0));
            }
            else{
#if defined(CS_ABORT_ON_UNKNOWN_ELEMENT) && CS_ABORT_ON_UNKNOWN_ELEMENT == 1
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
        delete sl;
        sl = nullptr;
    }

    return sl;
}


bool CockpitSerializer::writeSDOValueBar(QXmlStreamWriter& xml, const SDOValueBar* br, const QRect& pos) const
{
    xml.writeStartElement("SDOValueBar");
    writePosAttribs(xml, pos);
    if(auto sdoval = br->getSDOValue()){
        writeCoAttribs(xml, sdoval->nodeId(), sdoval->index(),
                       sdoval->subIndex(), br->SDOValueType());
    }

    xml.writeTextElement("name", br->name());
    xml.writeTextElement("barBackColor", QString::number(br->barBackColor().rgb()));
    xml.writeTextElement("barColor", QString::number(br->barColor().rgb()));
    xml.writeTextElement("barAlarmColor", QString::number(br->barAlarmColor().rgb()));
    xml.writeTextElement("scaleColor", QString::number(br->scaleColor().rgb()));
    xml.writeTextElement("textColor", QString::number(br->textColor().rgb()));
    xml.writeTextElement("barWidth", QString::number(br->barWidth()));
    xml.writeTextElement("borderWidth", QString::number(br->borderWidth()));
    xml.writeTextElement("penWidth", QString::number(br->penWidth()));
    xml.writeTextElement("orientation", QString::number(br->orientation()));
    xml.writeTextElement("scalePosition", QString::number(br->scalePosition()));
    xml.writeTextElement("alarmLevel", QString::number(br->alarmLevel()));
    xml.writeTextElement("alarmEnabled", QString::number(br->alarmEnabled()));
    xml.writeTextElement("rangeMin", QString::number(br->rangeMin()));
    xml.writeTextElement("rangeMax", QString::number(br->rangeMax()));

    xml.writeEndElement();

    return true;
}

SDOValueBar* CockpitSerializer::readSDOValueBar(QXmlStreamReader& xml) const
{
    //qDebug() << "readSDOValueBar";

    if(!xml.isStartElement()) return nullptr;
    if(xml.name() != "SDOValueBar") return nullptr;

    SDOValueBar* br = new SDOValueBar(m_valsHolder);

    auto co = readCoAttribs(xml);

    if(!br->setSDOValue(std::get<0>(co), std::get<1>(co), std::get<2>(co), std::get<3>(co))){
        delete br;
        return nullptr;
    }

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        //qDebug() << "token:" << token << xml.name();

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
            if(name == "name"){
                br->setName(xml.readElementText());
            }
            else if(name == "barBackColor"){
                br->setBarBackColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "barColor"){
                br->setBarColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "barAlarmColor"){
                br->setBarAlarmColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "scaleColor"){
                br->setScaleColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "textColor"){
                br->setTextColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "barWidth"){
                br->setBarWidth(intValue(xml.readElementText(), 0));
            }
            else if(name == "borderWidth"){
                br->setBorderWidth(intValue(xml.readElementText(), 0));
            }
            else if(name == "penWidth"){
                br->setPenWidth(realValue(xml.readElementText(), 0));
            }
            else if(name == "orientation"){
                br->setOrientation(static_cast<Qt::Orientation>(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "scalePosition"){
                br->setScalePosition(static_cast<QwtThermo::ScalePosition>(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "alarmLevel"){
                br->setAlarmLevel(realValue(xml.readElementText(), 0));
            }
            else if(name == "alarmEnabled"){
                br->setAlarmEnabled(uintValue(xml.readElementText(), 0));
            }
            else if(name == "rangeMin"){
                br->setRangeMin(realValue(xml.readElementText(), 0));
            }
            else if(name == "rangeMax"){
                br->setRangeMax(realValue(xml.readElementText(), 0));
            }
            else{
#if defined(CS_ABORT_ON_UNKNOWN_ELEMENT) && CS_ABORT_ON_UNKNOWN_ELEMENT == 1
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
        delete br;
        br = nullptr;
    }

    return br;
}


bool CockpitSerializer::writeSDOValueButton(QXmlStreamWriter& xml, const SDOValueButton* btn, const QRect& pos) const
{
    xml.writeStartElement("SDOValueButton");
    writePosAttribs(xml, pos);
    if(auto sdoval = btn->getSDOValue()){
        writeCoAttribs(xml, sdoval->nodeId(), sdoval->index(),
                       sdoval->subIndex(), btn->SDOValueType());
    }

    xml.writeTextElement("text", btn->text());
    xml.writeTextElement("buttonColor", QString::number(btn->buttonColor().rgb()));
    xml.writeTextElement("borderColor", QString::number(btn->borderColor().rgb()));
    xml.writeTextElement("indicatorColor", QString::number(btn->indicatorColor().rgb()));
    xml.writeTextElement("activateColor", QString::number(btn->activateColor().rgb()));
    xml.writeTextElement("highlightColor", QString::number(btn->highlightColor().rgb()));
    xml.writeTextElement("textColor", QString::number(btn->textColor().rgb()));
    xml.writeTextElement("borderWidth", QString::number(btn->borderWidth()));
    xml.writeTextElement("indicatorEnabled", QString::number(btn->indicatorEnabled()));
    xml.writeTextElement("fontPointSize", QString::number(btn->fontPointSize()));
    xml.writeTextElement("fontCapitalization", QString::number(btn->fontCapitalization()));
    xml.writeTextElement("fontBold", QString::number(btn->fontBold()));
    xml.writeTextElement("indicatorCompare", QString::number(btn->indicatorCompare()));
    xml.writeTextElement("indicatorValue", QString::number(btn->indicatorValue()));
    xml.writeTextElement("activateValue", QString::number(btn->activateValue()));

    xml.writeEndElement();

    return true;
}

SDOValueButton* CockpitSerializer::readSDOValueButton(QXmlStreamReader& xml) const
{
    //qDebug() << "readSDOValueButton";

    if(!xml.isStartElement()) return nullptr;
    if(xml.name() != "SDOValueButton") return nullptr;

    SDOValueButton* btn = new SDOValueButton(m_valsHolder);

    auto co = readCoAttribs(xml);

    if(!btn->setSDOValue(std::get<0>(co), std::get<1>(co), std::get<2>(co), std::get<3>(co))){
        delete btn;
        return nullptr;
    }

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        //qDebug() << "token:" << token << xml.name();

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
            if(name == "text"){
                btn->setText(xml.readElementText());
            }
            else if(name == "buttonColor"){
                btn->setButtonColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "borderColor"){
                btn->setBorderColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "indicatorColor"){
                btn->setIndicatorColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "activateColor"){
                btn->setActivateColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "highlightColor"){
                btn->setHighlightColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "textColor"){
                btn->setTextColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "borderWidth"){
                btn->setBorderWidth(intValue(xml.readElementText(), 0));
            }
            else if(name == "indicatorEnabled"){
                btn->setIndicatorEnabled(uintValue(xml.readElementText(), 0));
            }
            else if(name == "fontPointSize"){
                btn->setFontPointSize(intValue(xml.readElementText(), 0));
            }
            else if(name == "fontCapitalization"){
                btn->setFontCapitalization(uintValue(xml.readElementText(), 0));
            }
            else if(name == "fontBold"){
                btn->setFontBold(uintValue(xml.readElementText(), 0));
            }
            else if(name == "indicatorCompare"){
                btn->setIndicatorCompare(static_cast<SDOValueButton::CompareType>(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "indicatorValue"){
                btn->setIndicatorValue(uintValue(xml.readElementText(), 0));
            }
            else if(name == "activateValue"){
                btn->setActivateValue(uintValue(xml.readElementText(), 0));
            }
            else{
#if defined(CS_ABORT_ON_UNKNOWN_ELEMENT) && CS_ABORT_ON_UNKNOWN_ELEMENT == 1
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
        delete btn;
        btn = nullptr;
    }

    return btn;
}


bool CockpitSerializer::writeSDOValueIndicator(QXmlStreamWriter& xml, const SDOValueIndicator* ind, const QRect& pos) const
{
    xml.writeStartElement("SDOValueIndicator");
    writePosAttribs(xml, pos);
    if(auto sdoval = ind->getSDOValue()){
        writeCoAttribs(xml, sdoval->nodeId(), sdoval->index(),
                       sdoval->subIndex(), ind->SDOValueType());
    }

    xml.writeTextElement("text", ind->text());
    xml.writeTextElement("backColor", QString::number(ind->backColor().rgb()));
    xml.writeTextElement("shadowColor", QString::number(ind->shadowColor().rgb()));
    xml.writeTextElement("indicatorColor", QString::number(ind->indicatorColor().rgb()));
    xml.writeTextElement("glareColor", QString::number(ind->glareColor().rgb()));
    xml.writeTextElement("textColor", QString::number(ind->textColor().rgb()));
    xml.writeTextElement("borderWidth", QString::number(ind->borderWidth()));
    xml.writeTextElement("fontPointSize", QString::number(ind->fontPointSize()));
    xml.writeTextElement("fontCapitalization", QString::number(ind->fontCapitalization()));
    xml.writeTextElement("fontBold", QString::number(ind->fontBold()));
    xml.writeTextElement("indicatorCompare", QString::number(ind->indicatorCompare()));
    xml.writeTextElement("indicatorValue", QString::number(ind->indicatorValue()));

    xml.writeEndElement();

    return true;
}

SDOValueIndicator* CockpitSerializer::readSDOValueIndicator(QXmlStreamReader& xml) const
{
    //qDebug() << "readSDOValueIndicator";

    if(!xml.isStartElement()) return nullptr;
    if(xml.name() != "SDOValueIndicator") return nullptr;

    SDOValueIndicator* ind = new SDOValueIndicator(m_valsHolder);

    auto co = readCoAttribs(xml);

    if(!ind->setSDOValue(std::get<0>(co), std::get<1>(co), std::get<2>(co), std::get<3>(co))){
        delete ind;
        return nullptr;
    }

    bool done = false;
    bool res = true;

    while(!xml.atEnd() && !done){
        auto token = xml.readNext();

        //qDebug() << "token:" << token << xml.name();

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
            if(name == "text"){
                ind->setText(xml.readElementText());
            }
            else if(name == "backColor"){
                ind->setBackColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "shadowColor"){
                ind->setShadowColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "indicatorColor"){
                ind->setIndicatorColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "glareColor"){
                ind->setGlareColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "textColor"){
                ind->setTextColor(QColor::fromRgb(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "borderWidth"){
                ind->setBorderWidth(intValue(xml.readElementText(), 0));
            }
            else if(name == "fontPointSize"){
                ind->setFontPointSize(intValue(xml.readElementText(), 0));
            }
            else if(name == "fontCapitalization"){
                ind->setFontCapitalization(uintValue(xml.readElementText(), 0));
            }
            else if(name == "fontBold"){
                ind->setFontBold(uintValue(xml.readElementText(), 0));
            }
            else if(name == "indicatorCompare"){
                ind->setIndicatorCompare(static_cast<SDOValueIndicator::CompareType>(uintValue(xml.readElementText(), 0)));
            }
            else if(name == "indicatorValue"){
                ind->setIndicatorValue(uintValue(xml.readElementText(), 0));
            }
            else{
#if defined(CS_ABORT_ON_UNKNOWN_ELEMENT) && CS_ABORT_ON_UNKNOWN_ELEMENT == 1
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
        delete ind;
        ind = nullptr;
    }

    return ind;
}
