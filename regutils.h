#ifndef REGUTILS_H
#define REGUTILS_H

#include <QString>
#include <QPair>


namespace RegUtils{

// //! Получает систему счисления числа в строке.
//extern int numberBase(const QString& str, bool* ok = nullptr);

//! Получает строковое представление индекса и под-индекса.
extern QString indexSubIndexToString(uint index, uint subindex);

//! Получает индекс и под-индекс из строки.
extern QPair<uint, uint> indexSubIndexFromString(const QString& index_str, bool* ok = nullptr);

}

#endif // REGUTILS_H
