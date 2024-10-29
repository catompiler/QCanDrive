#ifndef REGLISTEXPORTER_H
#define REGLISTEXPORTER_H

#include <QObject>
#include "reglistmodel.h"

class QString;


class RegListExporter : public QObject
{
    Q_OBJECT
public:
    explicit RegListExporter(QObject *parent = nullptr);
    virtual ~RegListExporter();

    virtual bool doExport(const QString& filename, const RegEntryList* reglist) = 0;

signals:

};

#endif // REGLISTEXPORTER_H
