#include "regsviewdelegate.h"
#include "regsviewmodel.h"
#include "reglistmodel.h"
#include "regtypes.h"
#include "regutils.h"
#include "regentry.h"
#include "regvar.h"
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QString>
#include <QDebug>



RegsViewDelegate::RegsViewDelegate()
{
}

RegsViewDelegate::~RegsViewDelegate()
{

}

QWidget* RegsViewDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);

    //qDebug() << "RegViewDelegate::createEditor";

    if(!index.isValid()) return nullptr;

    const RegsViewModel* regsViewModel = qobject_cast<const RegsViewModel*>(index.model());
    if(regsViewModel == nullptr) return nullptr;

    RegListModel* regListModel = qobject_cast<RegListModel*>(regsViewModel->sourceModel());
    if(regListModel == nullptr) return nullptr;

    QModelIndex srcIndex = regsViewModel->mapToSource(regsViewModel->index(index.row(), 0, index.parent()));
    RegObject* ro = regListModel->objectByModelIndex(srcIndex);
    if(ro == nullptr) return nullptr;

    bool isEntry = ro->parent() == nullptr;
    if(isEntry) return nullptr; // Редактируемых значений нет у корневых элементов.

    //RegEntry* re = static_cast<RegEntry*>(ro->parent());
    RegVar* rv = static_cast<RegVar*>(ro);

    QWidget* res_widget = nullptr;

    RegsViewModel::ColId col_id = static_cast<RegsViewModel::ColId>(index.column());

    switch(col_id){
    default:
        break;
    case RegsViewModel::COL_VALUE:{
        switch(rv->dataType()){
        case DataType::I32:{
            QSpinBox* sb = new QSpinBox(parent);
            sb->setMinimum(INT32_MIN);
            sb->setMaximum(INT32_MAX);
            sb->setSingleStep(1);
            res_widget = sb;
            break;
        }
        case DataType::I16:{
            QSpinBox* sb = new QSpinBox(parent);
            sb->setMinimum(INT16_MIN);
            sb->setMaximum(INT16_MAX);
            sb->setSingleStep(1);
            res_widget = sb;
            break;
        }
        case DataType::I8:{
            QSpinBox* sb = new QSpinBox(parent);
            sb->setMinimum(INT8_MIN);
            sb->setMaximum(INT8_MAX);
            sb->setSingleStep(1);
            res_widget = sb;
            break;
        }
        case DataType::U32:{
            QDoubleSpinBox* dsb = new QDoubleSpinBox(parent);
            dsb->setMinimum(static_cast<double>(0));
            dsb->setMaximum(static_cast<double>(UINT32_MAX));
            dsb->setSingleStep(1);
            dsb->setDecimals(0);
            res_widget = dsb;
            break;
        }
        case DataType::U16:{
            QSpinBox* sb = new QSpinBox(parent);
            sb->setMinimum(0);
            sb->setMaximum(UINT16_MAX);
            sb->setSingleStep(1);
            res_widget = sb;
            break;
        }
        case DataType::U8:{
            QSpinBox* sb = new QSpinBox(parent);
            sb->setMinimum(0);
            sb->setMaximum(UINT8_MAX);
            sb->setSingleStep(1);
            res_widget = sb;
            break;
        }
        case DataType::IQ24:{
            QDoubleSpinBox* dsb = new QDoubleSpinBox(parent);
            dsb->setMinimum(static_cast<double>(INT32_MIN) / (1 << 24));
            dsb->setMaximum(static_cast<double>(INT32_MAX) / (1 << 24));
            dsb->setSingleStep(0.1);
            dsb->setDecimals(7);
            res_widget = dsb;
            break;
        }
        case DataType::IQ15:{
            QDoubleSpinBox* dsb = new QDoubleSpinBox(parent);
            dsb->setMinimum(static_cast<double>(INT32_MIN) / (1 << 15));
            dsb->setMaximum(static_cast<double>(INT32_MAX) / (1 << 15));
            dsb->setSingleStep(0.1);
            dsb->setDecimals(4);
            res_widget = dsb;
            break;
        }
        case DataType::IQ7:{
            QDoubleSpinBox* dsb = new QDoubleSpinBox(parent);
            dsb->setMinimum(static_cast<double>(INT32_MIN) / (1 << 7));
            dsb->setMaximum(static_cast<double>(INT32_MAX) / (1 << 7));
            dsb->setSingleStep(0.1);
            dsb->setDecimals(2);
            res_widget = dsb;
            break;
        }
        case DataType::STR:
        case DataType::MEM:
        default:{
            QLineEdit* le = new QLineEdit(parent);
            res_widget = le;
            break;
        }
        }
    }break;
    }

    return res_widget;
}

void RegsViewDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(!index.isValid()) return;

    const RegsViewModel* regsViewModel = qobject_cast<const RegsViewModel*>(index.model());
    if(regsViewModel == nullptr) return;

    RegListModel* regListModel = qobject_cast<RegListModel*>(regsViewModel->sourceModel());
    if(regListModel == nullptr) return;

    QModelIndex srcIndex = regsViewModel->mapToSource(regsViewModel->index(index.row(), 0, index.parent()));
    RegObject* ro = regListModel->objectByModelIndex(srcIndex);
    if(ro == nullptr) return;

    bool isEntry = ro->parent() == nullptr;
    if(isEntry) return; // Редактируемых значений нет у корневых элементов.

    //RegEntry* re = static_cast<RegEntry*>(ro->parent());
    RegVar* rv = static_cast<RegVar*>(ro);

    RegsViewModel::ColId col_id = static_cast<RegsViewModel::ColId>(index.column());

    switch(col_id){
    default:
        break;
    case RegsViewModel::COL_VALUE:{
        switch(rv->dataType()){
        case DataType::I32:
        case DataType::I16:
        case DataType::I8:
        case DataType::U16:
        case DataType::U8:{
            QSpinBox* sb = qobject_cast<QSpinBox*>(editor);
            if(sb == nullptr) break;
            sb->setValue(index.data(Qt::EditRole).toInt());
            break;
        }
        case DataType::U32:
        case DataType::IQ24:
        case DataType::IQ15:
        case DataType::IQ7:{
            QDoubleSpinBox* dsb = qobject_cast<QDoubleSpinBox*>(editor);
            if(dsb == nullptr) break;
            dsb->setValue(index.data(Qt::EditRole).toDouble());
            break;
        }
        case DataType::STR:
        case DataType::MEM:
        default:{
            QLineEdit* le = qobject_cast<QLineEdit*>(editor);
            if(le == nullptr) break;
            le->setText(index.data(Qt::EditRole).toString());
            break;
        }
        }
    }break;
    }
}

/*
QSpinBox* sb = qobject_cast<QSpinBox*>(editor);
if(sb == nullptr) break;
regListModel->setData(index, sb->value(), Qt::EditRole);
 */

void RegsViewDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if(!index.isValid()) return;

    RegsViewModel* regsViewModel = qobject_cast<RegsViewModel*>(model);
    if(regsViewModel == nullptr) return;

    RegListModel* regListModel = qobject_cast<RegListModel*>(regsViewModel->sourceModel());
    if(regListModel == nullptr) return;

    QModelIndex srcIndex = regsViewModel->mapToSource(regsViewModel->index(index.row(), 0, index.parent()));
    RegObject* ro = regListModel->objectByModelIndex(srcIndex);
    if(ro == nullptr) return;

    bool isEntry = ro->parent() == nullptr;
    if(isEntry) return; // Редактируемых значений нет у корневых элементов.

    //RegEntry* re = static_cast<RegEntry*>(ro->parent());
    RegVar* rv = static_cast<RegVar*>(ro);

    RegsViewModel::ColId col_id = static_cast<RegsViewModel::ColId>(index.column());

    switch(col_id){
    default:
        break;
    case RegsViewModel::COL_VALUE:{
        switch(rv->dataType()){
        case DataType::I32:
        case DataType::I16:
        case DataType::I8:
        case DataType::U16:
        case DataType::U8:{
            QSpinBox* sb = qobject_cast<QSpinBox*>(editor);
            if(sb == nullptr) break;
            regsViewModel->setData(index, sb->value(), Qt::EditRole);
            break;
        }
        case DataType::U32:
        case DataType::IQ24:
        case DataType::IQ15:
        case DataType::IQ7:{
            QDoubleSpinBox* dsb = qobject_cast<QDoubleSpinBox*>(editor);
            if(dsb == nullptr) break;
            regsViewModel->setData(index, dsb->value(), Qt::EditRole);
            break;
        }
        case DataType::STR:
        case DataType::MEM:
        default:{
            QLineEdit* le = qobject_cast<QLineEdit*>(editor);
            if(le == nullptr) break;
            regsViewModel->setData(index, le->text(), Qt::EditRole);
            break;
        }
        }
    }break;
    }
}

void RegsViewDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);
}
