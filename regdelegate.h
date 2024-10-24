#ifndef REGDELEGATE_H
#define REGDELEGATE_H

#include <QItemDelegate>


class RegDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    RegDelegate();
    ~RegDelegate();

    // QAbstractItemDelegate interface
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};

#endif // REGDELEGATE_H
