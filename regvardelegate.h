#ifndef REGVARDELEGATE_H
#define REGVARDELEGATE_H

#include <QItemDelegate>

class RegVarDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    RegVarDelegate();
    ~RegVarDelegate();

    // QAbstractItemDelegate interface
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};

#endif // REGVARDELEGATE_H
