#include "sdoscopechanneldelegate.h"
#include "sdoscopechannelsmodel.h"
#include "regselectdlg.h"
#include "sellineedit.h"
#include "regtypes.h"
#include "regutils.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QString>
#include <QVector>
#include <QDebug>




SDOScopeChannelDelegate::SDOScopeChannelDelegate(QObject *parent)
    : QItemDelegate{parent}
{
    m_regSelectDialog = nullptr;
}

SDOScopeChannelDelegate::~SDOScopeChannelDelegate()
{

}

QWidget* SDOScopeChannelDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);

    if(!index.isValid()) return nullptr;

    const SDOScopeChannelsModel* channelModel = qobject_cast<const SDOScopeChannelsModel*>(index.model());
    if(channelModel == nullptr) return nullptr;

    SDOScopeChannelsModel::ColId col_id = static_cast<SDOScopeChannelsModel::ColId>(index.column());

    QWidget* res_widget = nullptr;

    switch(col_id){
    default:
        break;
    case SDOScopeChannelsModel::COL_ENABLED:{
        QComboBox* cb = new QComboBox(parent);
        cb->addItems(RegTypes::boolStringList());
        res_widget = cb;
    }break;
    case SDOScopeChannelsModel::COL_NAME:{
        QLineEdit* le = new QLineEdit(parent);
        res_widget = le;
    }break;
    case SDOScopeChannelsModel::COL_REG:{
        SelLineEdit* le = new SelLineEdit(parent);
        connect(le, &SelLineEdit::select, this, &SDOScopeChannelDelegate::selectReg);
        connect(le, &SelLineEdit::editingFinished, this, &SDOScopeChannelDelegate::editingFinished);
        res_widget = le;
    }break;
    case SDOScopeChannelsModel::COL_DATA_TYPE:{
        QComboBox* cb = new QComboBox(parent);
        auto dataTypes = RegTypes::dataTypes();
        dataTypes.removeAll(DataType::MEM);
        dataTypes.removeAll(DataType::STR);
        for(auto& t: std::as_const(dataTypes)){
            cb->addItem(RegTypes::dataTypeStr(t));
        }
        res_widget = cb;
    }break;
    case SDOScopeChannelsModel::COL_BASE_VALUE:{
        QDoubleSpinBox* dsb = new QDoubleSpinBox(parent);
        dsb->setMinimum(0.0);
        dsb->setMaximum(INT32_MAX);
        res_widget = dsb;
    }break;
    }

    return res_widget;
}

void SDOScopeChannelDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(!index.isValid()) return;

    const SDOScopeChannelsModel* channelModel = qobject_cast<const SDOScopeChannelsModel*>(index.model());
    if(channelModel == nullptr) return;

    QVariant data = index.data(Qt::EditRole);

    SDOScopeChannelsModel::ColId col_id = static_cast<SDOScopeChannelsModel::ColId>(index.column());

    switch(col_id){
    default:
        break;
    case SDOScopeChannelsModel::COL_ENABLED:{
        QComboBox* cb = qobject_cast<QComboBox*>(editor);
        if(cb == nullptr) break;
        cb->setCurrentIndex(static_cast<int>(data.toBool()));
    }break;
    case SDOScopeChannelsModel::COL_NAME:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        le->setText(data.toString());
    }break;
    case SDOScopeChannelsModel::COL_REG:{
        SelLineEdit* le = qobject_cast<SelLineEdit*>(editor);
        if(le == nullptr) break;
        unsigned int reg = data.toUInt();
        unsigned int reg_index = RegUtils::getIndex(reg);
        unsigned int reg_subindex = RegUtils::getSubIndex(reg);
        le->setText(RegUtils::indexSubIndexToString(reg_index, reg_subindex));
    }break;
    case SDOScopeChannelsModel::COL_DATA_TYPE:{
        QComboBox* cb = qobject_cast<QComboBox*>(editor);
        if(cb == nullptr) break;
        cb->setCurrentIndex(static_cast<int>(data.toInt()));
    }break;
    case SDOScopeChannelsModel::COL_BASE_VALUE:{
        QDoubleSpinBox* dsb = qobject_cast<QDoubleSpinBox*>(editor);
        if(dsb == nullptr) break;
        dsb->setValue(data.toReal());
    }break;
    }
}

void SDOScopeChannelDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if(!index.isValid()) return;

    SDOScopeChannelsModel* channelModel = qobject_cast<SDOScopeChannelsModel*>(model);
    if(channelModel == nullptr) return;

    SDOScopeChannelsModel::ColId col_id = static_cast<SDOScopeChannelsModel::ColId>(index.column());

    switch(col_id){
    default:
        break;
    case SDOScopeChannelsModel::COL_ENABLED:{
        QComboBox* cb = qobject_cast<QComboBox*>(editor);
        if(cb == nullptr) break;
        channelModel->setData(index, cb->currentIndex(), Qt::EditRole);
    }break;
    case SDOScopeChannelsModel::COL_NAME:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        if(le->text().isEmpty()) break;
        channelModel->setData(index, le->text(), Qt::EditRole);
    }break;
    case SDOScopeChannelsModel::COL_REG:{
        SelLineEdit* le = qobject_cast<SelLineEdit*>(editor);
        if(le == nullptr) break;

        bool ok = false;
        auto index_pair = RegUtils::indexSubIndexFromString(le->text(), &ok);

        if(ok){
            unsigned int reg = RegUtils::makeFullIndex(index_pair.first, index_pair.second);
            channelModel->setData(index, reg, Qt::EditRole);
        }
    }break;
    case SDOScopeChannelsModel::COL_DATA_TYPE:{
        QComboBox* cb = qobject_cast<QComboBox*>(editor);
        if(cb == nullptr) break;
        channelModel->setData(index, cb->currentIndex(), Qt::EditRole);
    }break;
    case SDOScopeChannelsModel::COL_BASE_VALUE:{
        QDoubleSpinBox* dsb = qobject_cast<QDoubleSpinBox*>(editor);
        if(dsb == nullptr) break;
        channelModel->setData(index, dsb->value(), Qt::EditRole);
    }break;
    }
}

void SDOScopeChannelDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);
}

RegSelectDlg* SDOScopeChannelDelegate::regSelectDialog() const
{
    return m_regSelectDialog;
}

void SDOScopeChannelDelegate::setRegSelectDialog(RegSelectDlg* newRegSelectDialog)
{
    m_regSelectDialog = newRegSelectDialog;
}

void SDOScopeChannelDelegate::selectReg()
{
    SelLineEdit* le = qobject_cast<SelLineEdit*>(sender());
    if(le == nullptr) return;

    if(m_regSelectDialog == nullptr) return;

    bool ok = false;
    auto index_pair = RegUtils::indexSubIndexFromString(le->text(), &ok);

    if(ok){
        m_regSelectDialog->selectReg(index_pair.first, index_pair.second);
    }

    if(m_regSelectDialog->exec()){
        if(m_regSelectDialog->hasSelectedReg()){
            auto sel_pair = m_regSelectDialog->selectedRegIndex();
            le->setText(RegUtils::indexSubIndexToString(sel_pair.first, sel_pair.second));
        }
    }
}

void SDOScopeChannelDelegate::editingFinished()
{
    QWidget* w = qobject_cast<QWidget*>(sender());
    if(w == nullptr) return;

    emit commitData(w);
    emit closeEditor(w, QAbstractItemDelegate::NoHint);
}
