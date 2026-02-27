#ifndef REGSVIEWDELEGATE_H
#define REGSVIEWDELEGATE_H

#include <QItemDelegate>


class RegsViewDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    RegsViewDelegate();
    ~RegsViewDelegate();

    // QAbstractItemDelegate interface
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private slots:

private:
};

#endif // REGSVIEWDELEGATE_H
