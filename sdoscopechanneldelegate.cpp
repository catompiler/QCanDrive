#include "sdoscopechanneldelegate.h"
#include "sdoscopechannelsmodel.h"



SDOScopeChannelDelegate::SDOScopeChannelDelegate(QObject *parent)
    : QItemDelegate{parent}
{}

SDOScopeChannelDelegate::~SDOScopeChannelDelegate()
{

}

QWidget* SDOScopeChannelDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
}

void SDOScopeChannelDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
}

void SDOScopeChannelDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
}

void SDOScopeChannelDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);
}
