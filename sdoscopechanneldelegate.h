#ifndef SDOSCOPECHANNELDELEGATE_H
#define SDOSCOPECHANNELDELEGATE_H

#include <QItemDelegate>

class RegSelectDlg;


class SDOScopeChannelDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit SDOScopeChannelDelegate(QObject *parent = nullptr);
    ~SDOScopeChannelDelegate();

    // QAbstractItemDelegate interface
public:
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    RegSelectDlg* regSelectDialog() const;
    void setRegSelectDialog(RegSelectDlg* newRegSelectDialog);

private slots:
    void selectReg();
    void editingFinished();

private:
    RegSelectDlg* m_regSelectDialog;
};

#endif // SDOSCOPECHANNELDELEGATE_H
