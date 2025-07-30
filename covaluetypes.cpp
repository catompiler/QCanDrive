#include "covaluetypes.h"
#include <QObject>



QList<QPair<QString, COValue::Type>> COValue::getNumericTypesNames()
{
    QList<QPair<QString, COValue::Type>> types;

    types << qMakePair(QObject::tr("I32"), COValue::Type::I32)
          << qMakePair(QObject::tr("I16"), COValue::Type::I16)
          << qMakePair(QObject::tr("I8"), COValue::Type::I8)
          << qMakePair(QObject::tr("U32"), COValue::Type::U32)
          << qMakePair(QObject::tr("U16"), COValue::Type::U16)
          << qMakePair(QObject::tr("U8"), COValue::Type::U8)
          << qMakePair(QObject::tr("IQ24"), COValue::Type::IQ24)
          << qMakePair(QObject::tr("IQ15"), COValue::Type::IQ15)
          << qMakePair(QObject::tr("IQ7"), COValue::Type::IQ7);

    return types;
}
