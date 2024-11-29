#ifndef SIGNALCURVEPROPMODEL_H
#define SIGNALCURVEPROPMODEL_H

#include <QAbstractListModel>
#include "signalcurveprop.h"
#include <QVector>



class SignalCurvePropModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SignalCurvePropModel(QObject *parent = nullptr);
    ~SignalCurvePropModel();

    int propsCount() const;
    const SignalCurveProp& prop(int n) const;
    void setProp(int n, const SignalCurveProp& newProp);
    void addProp(const SignalCurveProp& newProp);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild) override;

private:
    QList<SignalCurveProp> m_signalsProps;

};

#endif // SIGNALCURVEPROPMODEL_H
