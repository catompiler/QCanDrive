#ifndef COCKPITSERIALIZER_H
#define COCKPITSERIALIZER_H

#include <QWidget>
#include <QRect>
#include <QPair>
#include <tuple>
#include "cotypes.h"
#include "covaluetypes.h"

class QIODevice;
class QString;
class QStringRef;
class SDOValuePlot;
class SDOValueDial;
class SDOValueSlider;
class SDOValueBar;
class SDOValueButton;
class SDOValueIndicator;
class QXmlStreamWriter;
class QXmlStreamReader;
class CoValuesHolder;

class CockpitSerializer
{
public:

    using CockpitWidgets = QList<QPair<QWidget*, QRect>>;

    CockpitSerializer();
    ~CockpitSerializer();

    CoValuesHolder* sdoValuesHolder() const;
    void setSdoValuesHolder(CoValuesHolder* newSdoValuesHolder);

    bool serialize(QIODevice* dev, const CockpitWidgets& widgets) const;
    CockpitWidgets deserialize(QIODevice* dev, bool* isOk = nullptr) const;

protected:

    int intValue(const QStringRef& str, int defVal = 0) const;
    int intValue(const QString& str, int defVal = 0) const;
    unsigned int uintValue(const QStringRef& str, unsigned int defVal = 0) const;
    unsigned int uintValue(const QString& str, unsigned int defVal = 0) const;
    double realValue(const QStringRef& str, double defVal = 0) const;
    double realValue(const QString& str, double defVal = 0) const;

    CockpitWidgets readCockpit(QXmlStreamReader& xml, bool* isOk = nullptr) const;
    bool writeCockpit(QXmlStreamWriter& xml, const CockpitWidgets& widgets) const;

    QRect readPosAttribs(QXmlStreamReader& xml) const;
    void writePosAttribs(QXmlStreamWriter& xml, const QRect& pos) const;

    std::tuple<CO::NodeId, CO::Index, CO::SubIndex, COValue::Type> readCoAttribs(QXmlStreamReader& xml) const;
    void writeCoAttribs(QXmlStreamWriter& xml, CO::NodeId nid, CO::Index idx, CO::SubIndex sidx, COValue::Type valtype) const;

    bool writeSDOValuePlot(QXmlStreamWriter& xml, const SDOValuePlot* plt, const QRect& pos) const;
    SDOValuePlot* readSDOValuePlot(QXmlStreamReader& xml) const;
    bool readSDOValuePlotSignalCurve(QXmlStreamReader& xml, SDOValuePlot* plt, int signum) const;

    bool writeSDOValueDial(QXmlStreamWriter& xml, const SDOValueDial* dial, const QRect& pos) const;
    SDOValueDial* readSDOValueDial(QXmlStreamReader& xml) const;

    bool writeSDOValueSlider(QXmlStreamWriter& xml, const SDOValueSlider* slider, const QRect& pos) const;
    SDOValueSlider* readSDOValueSlider(QXmlStreamReader& xml) const;

    bool writeSDOValueBar(QXmlStreamWriter& xml, const SDOValueBar* bar, const QRect& pos) const;
    SDOValueBar* readSDOValueBar(QXmlStreamReader& xml) const;

    bool writeSDOValueButton(QXmlStreamWriter& xml, const SDOValueButton* btn, const QRect& pos) const;
    SDOValueButton* readSDOValueButton(QXmlStreamReader& xml) const;

    bool writeSDOValueIndicator(QXmlStreamWriter& xml, const SDOValueIndicator* btn, const QRect& pos) const;
    SDOValueIndicator* readSDOValueIndicator(QXmlStreamReader& xml) const;

    CoValuesHolder* m_valsHolder;
};

#endif // COCKPITSERIALIZER_H
