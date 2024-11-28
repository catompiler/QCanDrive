#ifndef SDOVALUEPLOT_H
#define SDOVALUEPLOT_H

#include "signalplot.h"
#include "cotypes.h"
#include "covaluesholder.h"
#include "covaluetypes.h"
#include <stddef.h>
#include <variant>
#include <QList>
#include <QPair>
#include <QElapsedTimer>



class SDOValuePlot : public SignalPlot
{
    Q_OBJECT
public:
    SDOValuePlot(const QString& newName = QString(), CoValuesHolder* valsHolder = nullptr, QWidget* parent = nullptr);
    ~SDOValuePlot();

    CoValuesHolder* valuesHolder() const;
    void setValuesHolder(CoValuesHolder* newValuesHolder);

    bool addSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, const std::variant<COValue::Type, size_t>& sizeOrType,
                     const QString& newName = QString(), const QColor& newColor = QColor(), const qreal& z = -1);
    size_t SDOValuesCount() const;
    CoValuesHolder::HoldedSDOValuePtr SDOValue(size_t n) const;
    void delSDOValue(CoValuesHolder::HoldedSDOValuePtr sdoval);
    int SDOValueSignalNumber(size_t n) const;

private slots:
    void sdovalueReaded();
    void sdovalsUpdating();

protected:
    CoValuesHolder* m_valsHolder;

    struct SDOValItem{
        CoValuesHolder::HoldedSDOValuePtr sdoval;
        bool readed;
        std::variant<COValue::Type, size_t> sizeOrType;
        int signal_num;
        QElapsedTimer elapsedTimer;
    };

    QList<SDOValItem> m_sdoValues;

    void putValue(SDOValItem* sdoItem);
};

#endif // SDOVALUEPLOT_H
