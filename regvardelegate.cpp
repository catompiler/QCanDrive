#include "regvardelegate.h"
#include "regtypes.h"
#include "regvarmodel.h"
#include "regvar.h"
#include <QComboBox>
#include <QDebug>



RegVarDelegate::RegVarDelegate()
{

}

RegVarDelegate::~RegVarDelegate()
{

}

QWidget* RegVarDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    qDebug() << "RegVarDelegate::createEditor";

    if(!index.isValid() || index.column() != REGVARMODEL_COL_VAL){
        return QItemDelegate::createEditor(parent, option, index);
    }

    const RegVarModel* regVarModel = qobject_cast<const RegVarModel*>(index.model());

    if(regVarModel == nullptr){
        return QItemDelegate::createEditor(parent, option, index);
    }

    const RegVar* regVar = regVarModel->regVar();

    if(regVar == nullptr){
        return QItemDelegate::createEditor(parent, option, index);
    }

    QWidget* res_widget = nullptr;

    switch(index.row()){
    default:
        res_widget = QItemDelegate::createEditor(parent, option, index);
        break;
    case REGVARMODEL_ROW_DATATYPE:{
        QComboBox* cb_data_types = new QComboBox(parent);
        auto data_types = RegTypes::dataTypes();

        for(auto data_type: data_types){
            cb_data_types->addItem(RegTypes::dataTypeStr(data_type), static_cast<int>(data_type));
        }

        res_widget = static_cast<QWidget*>(cb_data_types);
    }break;
    case REGVARMODEL_ROW_MIN_VALUE:
    case REGVARMODEL_ROW_MAX_VALUE:
    case REGVARMODEL_ROW_DEF_VALUE:{
        switch(regVar->dataType()){
        case DataType::BOOLEAN:
        case DataType::INTEGER8:
        case DataType::INTEGER16:
        case DataType::INTEGER32:
        case DataType::INTEGER64:
        case DataType::UNSIGNED8:
        case DataType::UNSIGNED16:
        case DataType::UNSIGNED32:
        case DataType::UNSIGNED64:
        case DataType::REAL32:
        case DataType::REAL64:
        case DataType::VISIBLE_STRING:
        case DataType::OCTET_STRING:
        case DataType::UNICODE_STRING:
        case DataType::DOMAIN:
        default:
            res_widget = QItemDelegate::createEditor(parent, option, index);
            break;
        }
    }break;
    }

    return res_widget;
}

void RegVarDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(!index.isValid() || index.column() != REGVARMODEL_COL_VAL) return;

    const RegVarModel* regVarModel = qobject_cast<const RegVarModel*>(index.model());

    if(regVarModel == nullptr) return;

    const RegVar* regVar = regVarModel->regVar();

    if(regVar == nullptr) return;

    switch(index.row()){
    default:
        break;
    case REGVARMODEL_ROW_DATATYPE:{
        QComboBox* cb_data_types = qobject_cast<QComboBox*>(editor);
        if(cb_data_types == nullptr) break;

        for(int i = 0; i < cb_data_types->count(); i ++){
            if(cb_data_types->itemData(i, Qt::UserRole).toInt() == static_cast<int>(regVar->dataType())){
                cb_data_types->setCurrentIndex(i);
                break;
            }
        }
    }break;
    case REGVARMODEL_ROW_MIN_VALUE:
    case REGVARMODEL_ROW_MAX_VALUE:
    case REGVARMODEL_ROW_DEF_VALUE:{
        switch(regVar->dataType()){
        case DataType::BOOLEAN:
        case DataType::INTEGER8:
        case DataType::INTEGER16:
        case DataType::INTEGER32:
        case DataType::INTEGER64:
        case DataType::UNSIGNED8:
        case DataType::UNSIGNED16:
        case DataType::UNSIGNED32:
        case DataType::UNSIGNED64:
        case DataType::REAL32:
        case DataType::REAL64:
        case DataType::VISIBLE_STRING:
        case DataType::OCTET_STRING:
        case DataType::UNICODE_STRING:
        case DataType::DOMAIN:
        default:
            QItemDelegate::setEditorData(editor, index);
            break;
        }
    }break;
    }
}

void RegVarDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if(!index.isValid() || index.column() != REGVARMODEL_COL_VAL) return;

    const RegVarModel* regVarModel = qobject_cast<const RegVarModel*>(model);

    if(regVarModel == nullptr) return;

    RegVar* regVar = regVarModel->regVar();

    if(regVar == nullptr) return;

    switch(index.row()){
    default:
        break;
    case REGVARMODEL_ROW_DATATYPE:{
        QComboBox* cb_data_types = qobject_cast<QComboBox*>(editor);
        if(cb_data_types == nullptr) break;

        regVar->setDataType(static_cast<DataType>(cb_data_types->currentData(Qt::UserRole).toInt()));
    }break;
    case REGVARMODEL_ROW_MIN_VALUE:
    case REGVARMODEL_ROW_MAX_VALUE:
    case REGVARMODEL_ROW_DEF_VALUE:{
        switch(regVar->dataType()){
        case DataType::BOOLEAN:
        case DataType::INTEGER8:
        case DataType::INTEGER16:
        case DataType::INTEGER32:
        case DataType::INTEGER64:
        case DataType::UNSIGNED8:
        case DataType::UNSIGNED16:
        case DataType::UNSIGNED32:
        case DataType::UNSIGNED64:
        case DataType::REAL32:
        case DataType::REAL64:
        case DataType::VISIBLE_STRING:
        case DataType::OCTET_STRING:
        case DataType::UNICODE_STRING:
        case DataType::DOMAIN:
        default:
            QItemDelegate::setModelData(editor, model, index);
            break;
        }
    }break;
    }
}
