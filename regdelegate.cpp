#include "regdelegate.h"
#include "reglistmodel.h"
#include "regtypes.h"
#include "regvar.h"
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QDebug>



RegDelegate::RegDelegate()
{

}

RegDelegate::~RegDelegate()
{

}

QWidget* RegDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    qDebug() << "RegVarDelegate::createEditor";

    if(!index.isValid()){
        return QItemDelegate::createEditor(parent, option, index);
    }

    const RegListModel* regListModel = qobject_cast<const RegListModel*>(index.model());

    if(regListModel == nullptr){
        return QItemDelegate::createEditor(parent, option, index);
    }

    RegObject* ro = regListModel->objectByIndex(index);

    if(ro == nullptr){
        return QItemDelegate::createEditor(parent, option, index);
    }

    QWidget* res_widget = nullptr;

    RegListModel::ColId col_id = static_cast<RegListModel::ColId>(index.column());

    switch(col_id){
    case RegListModel::COL_INDEX:{
        QSpinBox* sb = new QSpinBox(parent);
        sb->setMinimum(0);
        sb->setMaximum(65536);
        sb->setDisplayIntegerBase(16);
        sb->setPrefix("0x");
        res_widget = sb;
    }break;
    case RegListModel::COL_NAME:{
        QLineEdit* le = new QLineEdit(parent);
        res_widget = le;
    }break;
    case RegListModel::COL_TYPE:{
        QComboBox* cb = new QComboBox(parent);
        auto objTypes = RegTypes::objectTypes();
        for(auto t: qAsConst(objTypes)){
            cb->addItem(RegTypes::typeStr(t));
        }
        res_widget = cb;
    }break;
    case RegListModel::COL_COUNT:{
        QSpinBox* sb = new QSpinBox(parent);
        sb->setMinimum(0);
        sb->setMaximum(254);
        res_widget = sb;
    }break;
    case RegListModel::COL_DATATYPE:{
        QComboBox* cb = new QComboBox(parent);
        auto dataTypes = RegTypes::dataTypes();
        for(auto t: qAsConst(dataTypes)){
            cb->addItem(RegTypes::dataTypeStr(t));
        }
        res_widget = cb;
    }break;
    case RegListModel::COL_MIN_VAL:
    case RegListModel::COL_MAX_VAL:
    case RegListModel::COL_DEF_VAL:{
        if(ro->type() == ObjectType::VAR){
            RegVar* var = static_cast<RegVar*>(ro);
            DataType data_type = var->dataType();
            switch(data_type){
            case DataType::BOOLEAN:{
                QComboBox* cb = new QComboBox(parent);
                cb->addItems(RegTypes::boolStringList());
                res_widget = cb;
            }break;
            case DataType::INTEGER8:
            case DataType::INTEGER16:
            case DataType::INTEGER32:
            case DataType::INTEGER64:
            case DataType::UNSIGNED8:
            case DataType::UNSIGNED16:
            case DataType::UNSIGNED32:
            case DataType::UNSIGNED64:{
                QLineEdit* le = new QLineEdit(parent);
                res_widget = le;
            }break;
            case DataType::REAL32:
            case DataType::REAL64:{
                QDoubleSpinBox* sb = new QDoubleSpinBox(parent);
                res_widget = sb;
            }break;
            case DataType::VISIBLE_STRING:
            case DataType::OCTET_STRING:
            case DataType::UNICODE_STRING:{
                QLineEdit* le = new QLineEdit(parent);
                res_widget = le;
            }break;
            default:
            case DataType::DOMAIN:{
                QLineEdit* le = new QLineEdit(parent);
                res_widget = le;
            }break;
            }
        }else{
            res_widget = QItemDelegate::createEditor(parent, option, index);
        }
    }break;
    case RegListModel::COL_FLAGS:
    case RegListModel::COL_EXTFLAGS:{
        QLineEdit* le = new QLineEdit(parent);
        le->setInputMask("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
        res_widget = le;
    }break;
    case RegListModel::COL_DESCR:{
        QLineEdit* le = new QLineEdit(parent);
        res_widget = le;
    }break;
    }

    return res_widget;
}

void RegDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(!index.isValid()) return;

    //const RegListModel* regListModel = qobject_cast<const RegListModel*>(index.model());

    //if(regListModel == nullptr) return;

    QVariant data = index.data(); //regListModel->data(index, Qt::EditRole);

    RegListModel::ColId col_id = static_cast<RegListModel::ColId>(index.column());

    switch(col_id){
    case RegListModel::COL_INDEX:{
        QSpinBox* sb = qobject_cast<QSpinBox*>(editor);
        if(sb == nullptr) break;
        sb->setValue(data.toInt());
    }break;
    case RegListModel::COL_NAME:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        le->setText(data.toString());
    }break;
    case RegListModel::COL_TYPE:{
        QComboBox* cb = qobject_cast<QComboBox*>(editor);
        if(cb == nullptr) break;
        cb->setCurrentIndex(static_cast<int>(data.toInt()));
    }break;
    case RegListModel::COL_COUNT:{
        QSpinBox* sb = qobject_cast<QSpinBox*>(editor);
        if(sb == nullptr) break;
        sb->setValue(data.toInt());
    }break;
    case RegListModel::COL_DATATYPE:{
        QComboBox* cb = qobject_cast<QComboBox*>(editor);
        if(cb == nullptr) break;
        cb->setCurrentIndex(static_cast<int>(data.toInt()));
    }break;
    case RegListModel::COL_MIN_VAL:
    case RegListModel::COL_MAX_VAL:
    case RegListModel::COL_DEF_VAL:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        le->setText(data.toString());
    }break;
    case RegListModel::COL_FLAGS:
    case RegListModel::COL_EXTFLAGS:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        le->setText(QString::number(data.toUInt(), 2));
    }break;
    case RegListModel::COL_DESCR:{
        QLineEdit* le = qobject_cast<QLineEdit*>(editor);
        if(le == nullptr) break;
        le->setText(data.toString());
    }break;
    }
}

void RegDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if(!index.isValid()) return;

    const RegListModel* regListModel = qobject_cast<const RegListModel*>(model);

    if(regListModel == nullptr) return;

    RegListModel::ColId col_id = static_cast<RegListModel::ColId>(index.column());

    switch(col_id){
    case RegListModel::COL_INDEX:
    case RegListModel::COL_NAME:
    case RegListModel::COL_TYPE:
    case RegListModel::COL_COUNT:
    case RegListModel::COL_DATATYPE:
    case RegListModel::COL_MIN_VAL:
    case RegListModel::COL_MAX_VAL:
    case RegListModel::COL_DEF_VAL:
    case RegListModel::COL_FLAGS:
    case RegListModel::COL_EXTFLAGS:
    case RegListModel::COL_DESCR:
        break;
    }

    DataType data_type = DataType::INTEGER32;

    switch(data_type){
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
}
