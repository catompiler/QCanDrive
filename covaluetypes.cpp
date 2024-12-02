#include "covaluetypes.h"
#include <QObject>



QList<QPair<QString, COValue::Type>> COValue::getTypesNames()
{
    QList<QPair<QString, COValue::Type>> types;

    types << qMakePair(QObject::tr("I32"), COValue::I32)
          << qMakePair(QObject::tr("I16"), COValue::I16)
          << qMakePair(QObject::tr("I8"), COValue::I8)
          << qMakePair(QObject::tr("U32"), COValue::U32)
          << qMakePair(QObject::tr("U16"), COValue::U16)
          << qMakePair(QObject::tr("U8"), COValue::U8)
          << qMakePair(QObject::tr("IQ24"), COValue::IQ24)
          << qMakePair(QObject::tr("IQ15"), COValue::IQ15)
          << qMakePair(QObject::tr("IQ7"), COValue::IQ7);

    return types;
}
