#ifndef PARAMSREADER_H
#define PARAMSREADER_H

#include <QObject>
#include <QMap>
#include "regutils.h"
#include "reglistmodel.h"


class ParamsReader : public QObject
{
    Q_OBJECT
public:

    using RegValuesList = QMap<reg_fullindex_t, int32_t>;

    explicit ParamsReader(QObject *parent = nullptr);
    ~ParamsReader();

    bool write(const QString& fileName, const RegVarList& varList, const RegValuesList valuesList) const;
    RegValuesList read(const QString& fileName, const RegVarList& varList) const;

signals:

private:
    bool writeVar(const RegVar* rv) const;
};

#endif // PARAMSREADER_H
