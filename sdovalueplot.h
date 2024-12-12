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

    bool addSDOValue(CO::NodeId newNodeId, CO::Index newIndex, CO::SubIndex newSubIndex, COValue::Type type,
                     const QString& newName = QString(), const QColor& newColor = QColor(), const qreal& z = -1);
    int SDOValuesCount() const;
    CoValuesHolder::HoldedSDOValuePtr getSDOValue(int n) const;
    void delSDOValue(int n);
    void delSDOValue(CoValuesHolder::HoldedSDOValuePtr sdoval);
    void delAllSDOValues();
    COValue::Type SDOValueType(int n) const;

private slots:
    void sdovalueReaded();
    void sdovalsUpdating();

protected:
    CoValuesHolder* m_valsHolder;

    struct SDOValItem{
        CoValuesHolder::HoldedSDOValuePtr sdoval;
        bool readed;
        COValue::Type type;
        QElapsedTimer elapsedTimer;
    };

    QList<SDOValItem> m_sdoValues;

    void putValue(int n);
};

#endif // SDOVALUEPLOT_H
