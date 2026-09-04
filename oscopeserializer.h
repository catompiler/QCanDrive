#ifndef OSCOPESERIALIZER_H
#define OSCOPESERIALIZER_H

#include <QString>
#include <QList>
#include <QColor>
#include <QPen>
#include "sdoscope.h"
#include "regtypes.h"
#include "cotypes.h"

class QIODevice;
class QXmlStreamWriter;
class QXmlStreamReader;


class OScopeSerializer
{
public:

    struct CommonConf{
        uint prescaler;
        uint samplesCount;
        uint histSamplesCount;
        qreal hDiv;
        qreal hOffset;
    };

    struct ChannelConf{
        // Параметры данных.
        bool enabled;
        CO::Index index;
        CO::SubIndex subIndex;
        DataType dataType;
        qreal baseValue;
        // Параметры отображения.
        qreal vDiv;
        qreal vOffset;
        bool visible;
        QString name;
        QColor penColor;
        Qt::PenStyle penStyle;
        qreal penWidthF;
    };

    struct TriggerConf{
        bool enabled;
        uint channel;
        SDOScope::TriggerType type;
        int32_t value;
    };

    struct FloatingCursorConf{
        bool enabled;
    };

    struct CursorsConf{
        FloatingCursorConf floating;
    };

    using ChannelsConf = QList<ChannelConf>;

    struct ScopeConf{
        CommonConf common;
        TriggerConf trigger;
        CursorsConf cursors;
        ChannelsConf channels;
    };


    OScopeSerializer();
    ~OScopeSerializer();

    bool serialize(QIODevice* dev, const ScopeConf& conf) const;
    bool deserialize(QIODevice* dev, ScopeConf& conf) const;

protected:

    bool writeOScope(QXmlStreamWriter& xml, const ScopeConf& conf) const;
    bool readOScope(QXmlStreamReader& xml, ScopeConf& conf) const;

    bool writeCommon(QXmlStreamWriter& xml, const CommonConf& com_conf) const;
    bool readCommon(QXmlStreamReader& xml, CommonConf& com_conf) const;

    bool writeTrigger(QXmlStreamWriter& xml, const TriggerConf& trig_conf) const;
    bool readTrigger(QXmlStreamReader& xml, TriggerConf& trig_conf) const;

    bool writeCursors(QXmlStreamWriter& xml, const CursorsConf& cur_conf) const;
    bool readCursors(QXmlStreamReader& xml, CursorsConf& cur_conf) const;

    bool writeFloatingCursor(QXmlStreamWriter& xml, const FloatingCursorConf& fl_conf) const;
    bool readFloatingCursor(QXmlStreamReader& xml, FloatingCursorConf& fl_conf) const;

    bool writeChannels(QXmlStreamWriter& xml, const ChannelsConf& chs_conf) const;
    bool readChannels(QXmlStreamReader& xml, ChannelsConf& chs_conf) const;

    bool writeChannel(QXmlStreamWriter& xml, const ChannelConf& ch_conf) const;
    bool readChannel(QXmlStreamReader& xml, ChannelsConf& chs_conf) const;

};

#endif // OSCOPESERIALIZER_H
